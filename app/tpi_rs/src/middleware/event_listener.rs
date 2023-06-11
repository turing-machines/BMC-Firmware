use std::collections::{HashMap, HashSet};

use evdev::InputEventKind;
use evdev::{Device, Key};
use log::{debug, warn};

/// Structure that listens for incoming device events on device [`DEVICE_PATH`]
/// Using a simple callback mechanism.
pub struct EventListener<T> {
    context: T,
    map: HashMap<(Key, i32), Box<dyn Fn(&'_ mut T) + Send + Sync>>,
    device_path: &'static str,
}

impl<T: Send + Sync + 'static> EventListener<T> {
    pub fn new(context: T, device_path: &'static str) -> Self {
        Self {
            map: HashMap::new(),
            context,
            device_path,
        }
    }

    pub fn add_action<F>(mut self, key: Key, value: i32, action: F) -> Self
    where
        F: Fn(&'_ mut T) + Send + Sync + 'static,
    {
        self.map.insert((key, value), Box::new(action));
        self
    }

    /// non-blocking call to start listening for events of interest.
    pub fn run(mut self) -> std::io::Result<()> {
        let device = Device::open(self.device_path)?;
        self.verify_required_keys(&device);

        let mut event_stream = device.into_event_stream()?;
        tokio::spawn(async move {
            while let Ok(event) = event_stream.next_event().await {
                if let InputEventKind::Key(x) = event.kind() {
                    if let Some(action) = self.map.get(&(x, event.value())) {
                        action(&mut self.context);
                    } else {
                        debug!("no handler defined for event {:?}", event);
                    }
                }
            }
        });
        Ok(())
    }

    fn verify_required_keys(&self, device: &Device) {
        let required_keys = self.map.keys().map(|(k, _)| *k).collect::<HashSet<Key>>();
        let verified_keys = device.supported_keys().map_or(HashSet::new(), |k| {
            k.iter()
                .filter(|k| required_keys.contains(k))
                .collect::<HashSet<Key>>()
        });

        if required_keys != verified_keys {
            warn!(
                "not all required keys are supported by linux subsystem. need {:?}, supported {:?}",
                required_keys,
                device.supported_keys()
            );
        } else {
            debug!(
                "keys {:#?} are all supported by the subsystem",
                required_keys
            );
        }
    }
}
