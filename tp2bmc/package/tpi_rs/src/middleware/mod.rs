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

impl From<u32> for NodeId {
    fn from(v: u32) -> Self {
        match v {
            0 => NodeId::Node1,
            1 => NodeId::Node2,
            2 => NodeId::Node3,
            3 => NodeId::Node4,
            x => panic!("node id {} does not exist", x),
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
