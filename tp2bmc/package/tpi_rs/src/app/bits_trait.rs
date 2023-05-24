use crate::middleware::NodeId;

pub trait ToBits {
    fn to_bits(&self) -> u8;
}

impl ToBits for u8 {
    fn to_bits(&self) -> u8 {
        *self
    }
}

impl ToBits for NodeId {
    fn to_bits(&self) -> u8 {
        if self == &NodeId::All {
            15
        } else {
            1 << *self as u8
        }
    }
}
