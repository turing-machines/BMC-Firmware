use super::gpio_definitions::*;
use super::NodeId;
use super::UsbMode;
use super::UsbRoute;
use anyhow::Context;
use gpiod::Active;
use gpiod::{Chip, Lines, Options, Output};
use log::trace;
use std::io::ErrorKind;
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
    usb_en: Lines<Output>,
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
    pub async fn new() -> anyhow::Result<Self> {
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
                "usb vbus"
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
                usb_en,
                [USB_PWEN, USB_SWITCH],
                Active::High,
                "usb channel switcher"
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
            (atx, [POWER_EN, SYS_LED], Active::High, "atx line"),
            (rtl_reset, [RTL_RESET], Active::Low, "Realtek switch reset")
        );
        instance.usb_vbus.set_values(0b1111u8)?;

        Ok(instance)
    }

    pub async fn set_atx_power(&self, on: bool) -> std::io::Result<()> {
        let mut values: u8 = 0b10;
        if on {
            values = !values;
        }

        self.atx.set_values(values)?;
        sleep(Duration::from_secs(1)).await;
        Ok(())
    }

    /// changes the configuration in such a way that
    pub fn select_usb(&self, node: NodeId) -> std::io::Result<()> {
        let values: u8 = match node {
            NodeId::Node1 => 0b1100,
            NodeId::Node2 => 0b1101,
            NodeId::Node3 => 0b0011,
            NodeId::Node4 => 0b0111,
            NodeId::All => return Err(std::io::Error::from(ErrorKind::Unsupported)),
        };
        self.usb_mux.set_values(values)
    }

    pub fn set_usb_route(&self, route: UsbRoute, mode: UsbMode) -> std::io::Result<()> {
        let route_bits: u8 = if route == UsbRoute::BMC { 0b1 } else { 0b0 };
        let mode_bits: u8 = if mode == UsbMode::Slave { 0b1 } else { 0b0 };

        let values: u8 = mode_bits | (route_bits << 1);
        trace!("set_route {:#04b}", values);
        self.usb_en.set_values(values)
    }

    /// Set given nodes into usb boot mode. When powering the node on with this usb_boot mode
    /// enabled, the given node will boot into USB mode. Typically means that booting of eMMC is
    /// disabled.
    pub async fn set_usb_boot(&self, node_state: u8) -> std::io::Result<()> {
        self.rpi_boot.set_values(node_state)
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
            self.mode.set_values(&current_node_state)?;
            sleep(Duration::from_millis(100)).await;
            self.enable.set_values(&current_node_state)?;
            sleep(Duration::from_millis(100)).await;
            if n != 4 {
                self.reset.set_values(&current_node_state)?;
            }

            sleep(Duration::from_secs(1)).await;
            prev_node_state = current_node_state;
        }
        Ok(())
    }

    pub async fn reset(&self, node: NodeId) -> std::io::Result<()> {
        let value = node.to_bitfield();
        self.reset.set_values(&value)?;
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
