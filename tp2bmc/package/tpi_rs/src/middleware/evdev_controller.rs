use evdev::EventStream;
use evdev::{Device, Key};
use log::error;

const DEVICE_PATH: &str = "/dev/input/event0";

pub fn linux_event_stream() -> std::io::Result<EventStream> {
    let device = Device::open(DEVICE_PATH)?;
    verify_required_keys(&device);
    device.into_event_stream()
}

fn verify_required_keys(device: &Device) {
    let required_keys: [Key; 3] = [Key::KEY_POWER, Key::KEY_1, Key::KEY_RESTART];
    let verified_keys = device.supported_keys().map_or(Vec::new(), |k| {
        k.iter()
            .filter(|k| required_keys.contains(k))
            .collect::<Vec<Key>>()
    });

    if required_keys != verified_keys.as_slice() {
        error!(
            "not all required keys are supported by linux subsystem. need {:?}",
            required_keys
        );
    }
}
