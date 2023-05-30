use crate::middleware::NodeId;
use std::path::PathBuf;
use std::{fmt, fs};

#[derive(Debug)]
pub enum FlashingError {
    InvalidArgs,
    DeviceNotFound,
    GpioError,
    UsbError,
    IoError,
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
            FlashingError::IoError => write!(f, "File IO error"),
            FlashingError::Timeout => write!(f, "The node did not respond in a long time"),
            FlashingError::ChecksumMismatch => {
                write!(f, "Failed to verify image after writing to the node")
            }
        }
    }
}

impl std::error::Error for FlashingError {}

/// Boot the Raspberry Pi node into emulating a USB Mass Storage Device (MSD)
/// Precondition: one, and only one, node should be set into RPIBOOT mode via GPIO.
pub(crate) fn boot_node_to_msd(_node: NodeId) -> Result<(), FlashingError> {
    let options = rustpiboot::Options {
        delay: 500 * 1000,
        ..Default::default()
    };

    rustpiboot::boot(options).map_err(|err| {
        log::error!("Failed to reboot node as USB MSD: {:?}", err);
        FlashingError::UsbError
    })
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

pub(crate) fn get_device_path(allowed_vendors: &[&str]) -> Result<PathBuf, FlashingError> {
    let contents = fs::read_dir("/dev/disk/by-id").map_err(|err| {
        log::error!("Failed to list devices: {}", err);
        FlashingError::IoError
    })?;

    let target_prefixes = allowed_vendors
        .iter()
        .map(|vendor| format!("usb-{}_", vendor))
        .collect::<Vec<String>>();

    let mut matching_devices = vec![];

    for entry in contents {
        let entry = match entry {
            Ok(entry) => entry,
            Err(err) => {
                log::warn!("Intermittent IO error while listing devices: {}", err);
                continue;
            }
        };

        let Ok(file_name) = entry.file_name().into_string() else {
            continue;
        };

        for prefix in &target_prefixes {
            if file_name.starts_with(prefix) {
                matching_devices.push(file_name.clone());
            }
        }
    }

    // Exclude partitions, i.e. turns [ "x-part2", "x-part1", "x", "y-part2", "y-part1", "y" ]
    // into ["x", "y"].
    let unique_root_devices = matching_devices
        .iter()
        .filter(|this| {
            !matching_devices
                .iter()
                .any(|other| this.starts_with(other) && *this != other)
        })
        .collect::<Vec<&String>>();

    let symlink = match unique_root_devices[..] {
        [] => {
            log::error!("No supported devices found");
            return Err(FlashingError::DeviceNotFound);
        }
        [device] => device.clone(),
        _ => {
            log::error!(
                "Several supported devices found: found {}, expected 1",
                unique_root_devices.len()
            );
            return Err(FlashingError::GpioError);
        }
    };

    fs::canonicalize(format!("/dev/disk/by-id/{}", symlink)).map_err(|err| {
        log::error!("Failed to read link: {}", err);
        FlashingError::IoError
    })
}
