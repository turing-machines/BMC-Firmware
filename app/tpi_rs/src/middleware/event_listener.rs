use std::collections::{HashMap, HashSet};

use evdev::InputEventKind;
use evdev::{Device, Key};
use futures::future::BoxFuture;
use log::{error, warn};

const DEVICE_PATH: &str = "/dev/input/event0";

/// Structure that listens for incoming device events on device [DEVICE_PATH]
/// Using a simple callback mechanism.
pub struct EventListener<T> {
    map: HashMap<
        (Key, i32),
        Box<dyn Fn(&'_ T) -> BoxFuture<'static, anyhow::Result<()>> + Send + Sync>,
    >,
    context: T,
}

impl<T: Send + Sync + Clone + 'static> EventListener<T> {
    pub fn new(context: T) -> Self {
        Self {
            map: HashMap::new(),
            context,
        }
    }

    pub fn add_action_async<F>(mut self, key: Key, value: i32, action: F) -> Self
    where
        F: Fn(&'_ T) -> BoxFuture<'static, anyhow::Result<()>> + Send + Sync + 'static,
    {
        self.map.insert((key, value), Box::new(action));
        self
    }

    /// non-blocking call to start listening for events of interest.
    pub fn run(self) -> std::io::Result<()> {
        let device = Device::open(DEVICE_PATH)?;
        self.verify_required_keys(&device);

        let mut event_stream = device.into_event_stream()?;
        tokio::spawn(async move {
            while let Ok(event) = event_stream.next_event().await {
                if let InputEventKind::Key(x) = event.kind() {
                    if let Some(action) = self.map.get(&(x, event.value())) {
                        if let Err(e) = action(&self.context).await {
                            error!("{}", e);
                        }
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
        }
    }
}
