use crate::middleware::NodeId;
use std::fmt;

#[allow(dead_code)]
#[derive(Debug)]
pub enum FlashingError {
    InvalidArgs,
    Timeout,
    ChecksumMismatch,
}

impl fmt::Display for FlashingError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            FlashingError::InvalidArgs => {
                write!(f, "A specified node does not exist or image is not valid")
            }
            FlashingError::Timeout => write!(f, "The node did not respond in a long time"),
            FlashingError::ChecksumMismatch => {
                write!(f, "Failed to verify image after writing to the node")
            }
        }
    }
}

impl std::error::Error for FlashingError {}

/// Boot the Raspberry Pi node into emulating a USB Mass Storage Device (MSD)
pub(crate) fn boot_node_to_msd(_node: NodeId) -> Result<(), FlashingError> {
    let options = rustpiboot::Options::default();

    // TODO: at this point, one, and only one, node should be set into RPIBOOT mode via GPIO.

    let res = rustpiboot::boot(options);

    log::debug!("res={:?}", res);

    Ok(())
}
