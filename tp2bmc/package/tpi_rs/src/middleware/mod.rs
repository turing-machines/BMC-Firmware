pub mod app_persistency;
pub mod evdev_controller;
mod gpio_definitions;
pub mod pin_controller;
pub mod usbboot;

#[repr(C)]
#[derive(Debug, Eq, PartialEq, Clone, Copy)]
pub enum NodeId {
    Node1,
    Node2,
    Node3,
    Node4,
    All,
}

impl TryFrom<u32> for NodeId {
    type Error = String;

    fn try_from(value: u32) -> Result<Self, Self::Error> {
        match value {
            0 => Ok(NodeId::Node1),
            1 => Ok(NodeId::Node2),
            2 => Ok(NodeId::Node3),
            3 => Ok(NodeId::Node4),
            x => Err(format!("node id {} does not exist", x)),
        }
    }
}

#[repr(C)]
#[derive(Debug, Eq, PartialEq, Clone, Copy)]
pub enum NodeType {
    RaspberryPi4,
    JetsonTx2,
    RK1,
}
