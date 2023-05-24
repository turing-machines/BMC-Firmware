use std::collections::HashMap;

use evdev::{Device, Key};
use evdev::{EventStream, InputEventKind};
use futures::future::BoxFuture;
use log::{error, warn};

const DEVICE_PATH: &str = "/dev/input/event0";

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

    pub fn run(self) -> std::io::Result<()> {
        let mut event_stream = linux_event_stream()?;
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
}

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
        warn!(
            "not all required keys are supported by linux subsystem. need {:?}, supported {:?}",
            required_keys,
            device.supported_keys()
        );
    }
}
