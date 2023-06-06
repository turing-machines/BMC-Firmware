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
        self.to_bitfield()
    }
}
