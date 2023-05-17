use super::gpio_definitions::*;
use super::NodeId;
use anyhow::Context;
use gpiod::Active;
use gpiod::AsValuesMut;
use gpiod::{Chip, Lines, Masked, Options, Output};
use std::io::ErrorKind;
use std::time::Duration;
use tokio::time::sleep;

#[derive(Debug, Eq, PartialEq)]
pub enum UsbMode {
    BMC,
    UsbA,
}

/// This middleware is responsible for managing the switching of nodes on the
/// board, including power regulators and USB multiplexers. Due to hardware
/// limitations, only one node can be connected over the USB bus at a time. This
/// structure the GPIOD device library internally.
pub struct PinController {
    usb_mux: Lines<Output>,
    rpi_boot: Lines<Output>,
    mode: Lines<Output>,
    enable: Lines<Output>,
    reset: Lines<Output>,
}

/// small helper macro which handles the code duplication of declaring lines.
macro_rules! create_output_lines {
    ($chip:ident, $(($name:ident, $pins: expr, $active: expr, $description: literal)),+) => {
        $(let $name = $chip
            .request_lines(Options::output($pins).active($active))
            .context(concat!("error intializing ", stringify!($name), " pins"))?;
        )*
    };
}

impl PinController {
    /// create a new Pin controller
    pub async fn new() -> anyhow::Result<Self> {
        let chip = Chip::new("/dev/gpiochip0").context("gpiod device")?;
        create_output_lines!(
            chip,
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
                mode,
                [MODE1_EN, MODE2_EN, MODE3_EN, MODE4_EN, POWER_EN],
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
            )
        );

        Ok(Self {
            usb_mux,
            rpi_boot,
            mode,
            enable,
            reset,
        })
    }

    pub async fn atx_power(&self, on: bool) -> std::io::Result<()> {
        self.mode
            .set_values(Masked::<u8>::default().with(4, Some(on)))?;
        sleep(Duration::from_secs(1)).await;
        Ok(())
    }

    /// changes the configuration in such a way that
    pub async fn switch_usb(&self, node: NodeId, connect_to: UsbMode) -> std::io::Result<()> {
        let values: u8 = match node {
            NodeId::Node1 => 0b0011,
            NodeId::Node2 => 0b1011,
            NodeId::Node3 => 0b1100,
            NodeId::Node4 => 0b1110,
            NodeId::All => return Err(std::io::Error::from(ErrorKind::Unsupported)),
        };

        if connect_to == UsbMode::BMC {
        } else {
        }

        self.usb_mux.set_values(values)
    }

    /// Set given node into usb boot mode. When powering the node on with this usb_boot mode
    /// enabled, the given node will boot into USB mode. Typically means that booting of eMMC is
    /// disabled.
    pub async fn usb_boot(&self, node: NodeId, on: bool) -> std::io::Result<()> {
        let mut bits = as_bits(node);
        if !on {
            bits = !bits;
        }

        let mask = Masked {
            bits,
            mask: as_bits(node),
        };
        self.rpi_boot.set_values(mask)
    }

    pub async fn power_node(&self, bits: u8, mask: u8) -> std::io::Result<()> {
        for n in 0..4 {
            let output_mask = mask & (1 << n);
            // continue if bit is not to be written
            if output_mask == 0 {
                continue;
            }

            let masked = Masked {
                bits,
                mask: output_mask,
            };
            self.mode.set_values(masked)?;
            sleep(Duration::from_millis(100)).await;
            self.enable.set_values(masked)?;
            sleep(Duration::from_secs(1)).await;
        }
        Ok(())
    }

    pub async fn reset(&self, node: NodeId) -> std::io::Result<()> {
        let mut mask = Masked {
            bits: as_bits(node),
            mask: as_bits(node),
        };
        self.reset.set_values(mask)?;
        sleep(Duration::from_secs(1)).await;
        mask.bits = !mask.bits;
        self.reset.set_values(mask)
    }
}

fn as_bits(node: NodeId) -> u8 {
    if node == NodeId::All {
        15
    } else {
        1 << node as u8
    }
}

impl std::fmt::Debug for PinController {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "PinController")
    }
}
