use super::gpio_definitions::*;
use super::NodeId;
use super::UsbMode;
use super::UsbRoute;
use anyhow::Context;
use gpiod::Active;
use gpiod::{Chip, Lines, Options, Output};
use log::trace;
use std::time::Duration;
use tokio::time::sleep;

const NODE_COUNT: u8 = 4;

/// This middleware is responsible for managing the switching of nodes on the
/// board, including power regulators and USB multiplexers. Due to hardware
/// limitations, only one node can be connected over the USB bus at a time. This
/// structure the GPIOD device library internally.
pub struct PinController {
    usb_vbus: Lines<Output>,
    usb_mux: Lines<Output>,
    usb_pwen: Lines<Output>,
    usb_switch: Lines<Output>,
    rpi_boot: Lines<Output>,
    mode: Lines<Output>,
    enable: Lines<Output>,
    reset: Lines<Output>,
    atx: Lines<Output>,
    rtl_reset: Lines<Output>,
}

/// small helper macro which handles the code duplication of declaring lines.
macro_rules! create_output_lines {
    ($chip: literal, $(($name:ident, $pins: expr, $active: expr, $description: literal)),+) => {
        {
            let chip = Chip::new($chip).context("gpiod device")?;
            $(let $name = chip
                .request_lines(Options::output($pins).active($active))
                .context(concat!("error intializing ", stringify!($name), " pins"))?;
                )*


            PinController {
                $($name),+
            }
        }
    };
}

impl PinController {
    /// create a new Pin controller
    pub fn new() -> anyhow::Result<Self> {
        let instance = create_output_lines!(
            "gpiochip0",
            (
                usb_vbus,
                [
                    PORT1_USB_VBUS,
                    PORT2_USB_VBUS,
                    PORT3_USB_VBUS,
                    PORT4_USB_VBUS
                ],
                Active::High,
                "USB ports Host/Device mode switch"
            ),
            (
                rpi_boot,
                [PORT1_RPIBOOT, PORT2_RPIBOOT, PORT3_RPIBOOT, PORT4_RPIBOOT],
                Active::Low,
                "rpi boot"
            ),
            (
                usb_mux,
                [USB_SEL1, USB_OE1, USB_SEL2, USB_OE2],
                Active::High,
                "usb channel switcher"
            ),
            (
                usb_pwen,
                [USB_PWEN],
                Active::Low,
                "USB-A 5V power output enable"
            ),
            (
                usb_switch,
                [USB_SWITCH],
                Active::High,
                "USB channel control"
            ),
            (
                mode,
                [MODE1_EN, MODE2_EN, MODE3_EN, MODE4_EN],
                Active::High,
                "5v power enable"
            ),
            (
                enable,
                [PORT1_EN, PORT2_EN, PORT3_EN, PORT4_EN],
                Active::Low,
                "power on/off signal"
            ),
            (
                reset,
                [PORT1_RST, PORT2_RST, PORT3_RST, PORT4_RST],
                Active::High,
                "reset group"
            ),
            (atx, [POWER_EN], Active::High, "atx line"),
            (rtl_reset, [RTL_RESET], Active::Low, "Realtek switch reset")
        );

        instance.usb_vbus.set_values(0b1111u8)?;

        Ok(instance)
    }

    pub async fn set_atx_power(&self, on: bool) -> std::io::Result<()> {
        if on {
            self.atx.set_values(0b1u8)?;
        } else {
            self.atx.set_values(0b0u8)?;
        }

        sleep(Duration::from_secs(1)).await;
        Ok(())
    }

    /// Select which node is active in the multiplexer (see PORTx in `set_usb_route()`)
    pub fn select_usb(&self, node: NodeId) -> std::io::Result<()> {
        let values: u8 = match node {
            NodeId::Node1 => 0b1100,
            NodeId::Node2 => 0b1101,
            NodeId::Node3 => 0b0011,
            NodeId::Node4 => 0b0111,
        };
        self.usb_mux.set_values(values)
    }

    /// Set which way the USB is routed: USB-A ↔ PORTx (`UsbRoute::UsbA`) or BMC ↔ PORTx
    /// (`UsbRoute::BMC`)
    pub fn set_usb_route(&self, route: UsbRoute) -> std::io::Result<()> {
        match route {
            UsbRoute::UsbA => {
                self.usb_switch.set_values(0_u8)?;
                self.usb_pwen.set_values(1_u8)?;
            }
            UsbRoute::BMC => {
                self.usb_switch.set_values(1_u8)?;
                self.usb_pwen.set_values(0_u8)?;
            }
        }

        Ok(())
    }

    /// Set which role a node has (`UsbMode::Host` or `UsbMode::Device`)
    pub fn set_usb_mode(&self, node: NodeId, mode: UsbMode, prev_mode: u8) -> std::io::Result<u8> {
        let new_mode = match mode {
            UsbMode::Host => prev_mode & node.to_inverse_bitfield(),
            UsbMode::Device => prev_mode | node.to_bitfield(),
        };

        self.usb_vbus.set_values(new_mode)?;

        Ok(new_mode)
    }

    /// Set the bitmask value of USB Device/Host mode of nodes (0 being Host, and 1 being Device).
    /// For example, 0b1101 equates to node 2 acting as a Host, and the rest as Devices.
    pub fn inner_set_usb_mode(&self, values: u8) -> std::io::Result<()> {
        self.usb_vbus.set_values(values)
    }

    /// Set given nodes into usb boot mode. When powering the node on with this mode enabled, the
    /// given node will boot into USB mode. Typically means that booting of eMMC is disabled.
    pub fn set_usb_boot(&self, node: NodeId) -> std::io::Result<()> {
        self.rpi_boot.set_values(node.to_bitfield())
    }

    /// Clear USB boot mode of all nodes
    pub fn clear_usb_boot(&self) -> std::io::Result<()> {
        self.rpi_boot.set_values(0_u8)
    }

    pub async fn set_power_node(
        &self,
        mut current_node_state: u8,
        new_node_state: u8,
    ) -> std::io::Result<()> {
        let mut prev_node_state = current_node_state;
        for n in 0..NODE_COUNT {
            let masked_new = new_node_state & (1 << n);
            current_node_state = (current_node_state & !(1 << n)) | masked_new;
            if current_node_state == prev_node_state {
                continue;
            }

            trace!("set_power_node {:#4b}", current_node_state);
            self.mode.set_values(current_node_state)?;
            sleep(Duration::from_millis(100)).await;
            self.enable.set_values(current_node_state)?;
            sleep(Duration::from_millis(100)).await;
            if n != 4 {
                self.reset.set_values(current_node_state)?;
            }

            sleep(Duration::from_secs(1)).await;
            prev_node_state = current_node_state;
        }
        Ok(())
    }

    pub async fn reset(&self, node: NodeId) -> std::io::Result<()> {
        let value = node.to_bitfield();
        self.reset.set_values(value)?;
        sleep(Duration::from_secs(1)).await;
        self.reset.set_values(0u8)
    }

    pub async fn rtl_reset(&self) -> std::io::Result<()> {
        self.rtl_reset.set_values(1u8)?;
        sleep(Duration::from_secs(1)).await;
        self.rtl_reset.set_values(0u8)
    }
}

impl std::fmt::Debug for PinController {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "PinController")
    }
}
