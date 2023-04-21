use crate::NodeId;
use anyhow::Context;
use futures::future::BoxFuture;
use futures::FutureExt;
use futures_concurrency::future::Race;
use gpio::GpioOut;
use std::io::Error;
use std::ops::Deref;
use tokio::time::Duration;

const GPIO_PIN_PD: u16 = 96;
const GPIO_PIN_PG: u16 = 192;

/// Helper macro that lazy initializes a static gpio output pin object.
/// These objects are wrapped inside a Result.
macro_rules! gpio_output_pins {
    ($($name:ident = $val:expr),+) => {
       $( static $name: std::sync::Mutex<Option<gpio::sysfs::SysFsGpioOutput>> = std::sync::Mutex::new(None);
        )+

        fn initialize_gpio_out() -> anyhow::Result<()> {
            $(
             *$name.lock().expect("poisoned mutex") = Some(gpio::sysfs::SysFsGpioOutput::open($val)?);
             )+
            Ok(())
        }
    };
}

gpio_output_pins! {
    PORT1_EN = GPIO_PIN_PD+11,
    PORT2_EN = GPIO_PIN_PD+10,
    PORT3_EN = GPIO_PIN_PD+9,
    PORT4_EN = GPIO_PIN_PD+8,

    PORT1_RST=  GPIO_PIN_PD+0,
    PORT2_RST=  GPIO_PIN_PD+20,
    PORT3_RST=  GPIO_PIN_PD+21,
    PORT4_RST=  GPIO_PIN_PD+22,
    POWER_EN = GPIO_PIN_PD+3,
    MODE1_EN = GPIO_PIN_PD+7,
    MODE2_EN = GPIO_PIN_PD+6,
    MODE3_EN = GPIO_PIN_PD+5,
    MODE4_EN = GPIO_PIN_PD+4,

    RTL_RESET = GPIO_PIN_PG+13,
    SYS_LED = GPIO_PIN_PG+8,
    RESET_LED = GPIO_PIN_PG+9,
    SYS_RESET = GPIO_PIN_PG+11,
    POWER_DETECT = GPIO_PIN_PG+10,
    POWER_BOARD= GPIO_PIN_PG+15
}

pub fn initialize_power() -> anyhow::Result<()> {
    initialize_gpio_out()
}

pub fn power_on(node: NodeId) -> BoxFuture<'static, Result<(), Error>> {
    let (mode, port) = match node {
        NodeId::Node1 => (&MODE1_EN, &PORT1_EN),
        NodeId::Node2 => (&MODE2_EN, &PORT2_EN),
        NodeId::Node3 => (&MODE3_EN, &PORT3_EN),
        NodeId::Node4 => (&MODE4_EN, &PORT4_EN),
        NodeId::All => {
            return async move {
                (
                    power_on(NodeId::Node1),
                    power_on(NodeId::Node2),
                    power_on(NodeId::Node3),
                    power_on(NodeId::Node4),
                )
                    .race()
                    .await
            }
            .boxed();
        }
    };

    async move {
        //let mut port = port.lock();
        //let mut mode = mode.lock();
        tokio::time::sleep(Duration::from_millis(100)).await;
        Ok(())
    }
    .boxed()
}

pub fn power_off(node: NodeId) -> BoxFuture<'static, Result<(), Error>> {
    todo!()
}
