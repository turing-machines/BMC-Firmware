use crate::middleware::NodeId;
use std::fmt;

#[derive(Debug)]
pub enum FlashingError {
    InvalidArgs,
    DeviceNotFound,
    GpioError,
    UsbError,
    Timeout,
    ChecksumMismatch,
}

impl fmt::Display for FlashingError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            FlashingError::InvalidArgs => {
                write!(f, "A specified node does not exist or image is not valid")
            }
            FlashingError::DeviceNotFound => write!(f, "Device not found"),
            FlashingError::GpioError => write!(f, "Error toggling GPIO lines"),
            FlashingError::UsbError => write!(f, "Error enumerating USB devices"),
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

pub(crate) fn check_only_one_device_present(supported: &[(u16, u16)]) -> Result<(), FlashingError> {
    let all_devices = rusb::DeviceList::new().map_err(|err| {
        log::error!("failed to get USB device list: {}", err);
        FlashingError::UsbError
    })?;
    let mut matches = 0;

    for device in all_devices.iter() {
        let Ok(desc) = device.device_descriptor() else {
            continue;
        };

        let this = (desc.vendor_id(), desc.product_id());

        if supported.iter().any(|&d| d == this) {
            matches += 1;
        }
    }

    match matches {
        1 => Ok(()),
        0 => {
            log::error!("No supported devices found");
            Err(FlashingError::DeviceNotFound)
        }
        n => {
            log::error!("Several supported devices found: found {}, expected 1", n);
            Err(FlashingError::GpioError)
        }
    }
}
