//! This module contains pin numbers of the connected gpios. A next step would
//! be to configure it in the dst as a new gpiochip
#![allow(dead_code)]
pub const GPIO_PIN_PG: u32 = 192;
pub const GPIO_PIN_PD: u32 = 96;

pub const RTL_RESET: u32 = GPIO_PIN_PG + 13;
pub const SYS_RESET: u32 = GPIO_PIN_PG + 11;
pub const POWER_DETECT: u32 = GPIO_PIN_PG + 10;
pub const POWER_BOARD: u32 = GPIO_PIN_PG + 15;

pub const PORT1_EN: u32 = GPIO_PIN_PD + 11;
pub const PORT2_EN: u32 = GPIO_PIN_PD + 10;
pub const PORT3_EN: u32 = GPIO_PIN_PD + 9;
pub const PORT4_EN: u32 = GPIO_PIN_PD + 8;

pub const MODE1_EN: u32 = GPIO_PIN_PD + 7;
pub const MODE2_EN: u32 = GPIO_PIN_PD + 6;
pub const MODE3_EN: u32 = GPIO_PIN_PD + 5;
pub const MODE4_EN: u32 = GPIO_PIN_PD + 4;
pub const POWER_EN: u32 = GPIO_PIN_PD + 3;

pub const PORT1_RST: u32 = GPIO_PIN_PD + 0;
pub const PORT2_RST: u32 = GPIO_PIN_PD + 20;
pub const PORT3_RST: u32 = GPIO_PIN_PD + 21;
pub const PORT4_RST: u32 = GPIO_PIN_PD + 22;

pub const PORT1_USB_VBUS: u32 = GPIO_PIN_PD + 19;
pub const PORT2_USB_VBUS: u32 = GPIO_PIN_PD + 18;
pub const PORT3_USB_VBUS: u32 = GPIO_PIN_PD + 17;
pub const PORT4_USB_VBUS: u32 = GPIO_PIN_PD + 16;

pub const PORT1_RPIBOOT: u32 = GPIO_PIN_PD + 15;
pub const PORT2_RPIBOOT: u32 = GPIO_PIN_PD + 14;
pub const PORT3_RPIBOOT: u32 = GPIO_PIN_PD + 12;
pub const PORT4_RPIBOOT: u32 = GPIO_PIN_PD + 13;

pub const USB_SEL1: u32 = GPIO_PIN_PG + 1;
pub const USB_SEL2: u32 = GPIO_PIN_PG + 0;
pub const USB_OE1: u32 = GPIO_PIN_PG + 2;
pub const USB_OE2: u32 = GPIO_PIN_PG + 3;

pub const USB_SWITCH: u32 = GPIO_PIN_PG + 5;
pub const USB_PWEN: u32 = GPIO_PIN_PG + 4;
