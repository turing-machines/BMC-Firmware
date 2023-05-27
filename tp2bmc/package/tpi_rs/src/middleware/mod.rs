pub mod app_persistency;
pub mod event_listener;
mod gpio_definitions;
pub mod pin_controller;
pub mod usbboot;

#[repr(C)]
#[derive(Debug, Eq, PartialEq, Clone, Copy, serde::Serialize, serde::Deserialize)]
pub enum NodeId {
    Node1,
    Node2,
    Node3,
    Node4,
    All,
}

impl TryFrom<i32> for NodeId {
    type Error = String;

    fn try_from(value: i32) -> Result<Self, Self::Error> {
        match value {
            0 => Ok(NodeId::Node1),
            1 => Ok(NodeId::Node2),
            2 => Ok(NodeId::Node3),
            3 => Ok(NodeId::Node4),
            x => Err(format!("node id {} does not exist", x)),
        }
    }
}

impl NodeId {
    pub fn to_bitfield(&self) -> u8 {
        if self == &NodeId::All {
            15
        } else {
            1 << *self as u8
        }
    }
}

#[repr(C)]
#[derive(Debug, Eq, PartialEq, Clone, Copy, serde::Serialize, serde::Deserialize)]
pub enum NodeType {
    RaspberryPi4,
    JetsonTx2,
    RK1,
}

#[derive(Debug, Eq, PartialEq, serde::Serialize, serde::Deserialize)]
pub enum UsbRoute {
    BMC,
    UsbA,
}

impl TryFrom<i32> for UsbRoute {
    type Error = String;

    fn try_from(value: i32) -> Result<Self, Self::Error> {
        match value {
            0 => Ok(UsbRoute::BMC),
            1 => Ok(UsbRoute::UsbA),
            x => Err(format!("usb route{} does not exist", x)),
        }
    }
}

#[derive(Debug, Eq, PartialEq, Clone, Copy, serde::Serialize, serde::Deserialize)]
pub enum UsbMode {
    Slave,
    Master,
}

impl TryFrom<i32> for UsbMode {
    type Error = String;

    fn try_from(value: i32) -> Result<Self, Self::Error> {
        match value {
            0 => Ok(UsbMode::Slave),
            1 => Ok(UsbMode::Master),
            x => Err(format!("usb mode {} does not exist", x)),
        }
    }
}
