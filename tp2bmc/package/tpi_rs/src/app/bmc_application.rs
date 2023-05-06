use crate::middleware::{
    app_persistency::ApplicationPersistency, evdev_controller::linux_event_stream,
    pin_controller::PinController, NodeId,
};
use anyhow::Context;
use std::ops::Deref;
use std::{mem::size_of, sync::Arc};

const POWER_STATE_KEY: &str = "power_state";

#[derive(Debug)]
pub struct BmcApplication {
    pin_controller: Arc<PinController>,
    app_db: ApplicationPersistency,
}

impl BmcApplication {
    pub async fn new() -> anyhow::Result<Self> {
        let pin_controller = Arc::new(PinController::new().await?);
        let app_db = ApplicationPersistency::new().await?;

        let instance = Self {
            pin_controller,
            app_db,
        };

        instance.initialize().await?;
        Ok(instance)
    }

    async fn initialize(&self) -> anyhow::Result<()> {
        // power on nodes
        if let Ok(power_state) = self.app_db.get::<u8>(POWER_STATE_KEY).await {
            self.node_power_internal(power_state, 15, 0, power_state)
                .await?;
        }

        self.run_event_listener();
        Ok(())
    }

    fn run_event_listener(&self) {
        tokio::spawn(async move {
            let mut event_stream = linux_event_stream().unwrap();
            while let Ok(event) = event_stream.next_event().await {
                todo!()
            }
        });
    }

    async fn node_power_internal(
        &self,
        node: u8,
        mask: u8,
        current_node_state: u8,
        new_node_state: u8,
    ) -> anyhow::Result<()> {
        if let Some(on) = Self::need_atx_change(current_node_state, new_node_state) {
            self.pin_controller.deref().atx_power(on).await?;
        }

        self.pin_controller
            .power_node(node, mask)
            .await
            .context("pin controller error")
    }

    fn need_atx_change(current_node_state: u8, next_node_state: u8) -> Option<bool> {
        if current_node_state == 0 && next_node_state > 0 {
            // power down
            Some(true)
        } else if current_node_state > 0 && next_node_state == 0 {
            // power up
            Some(false)
        } else {
            // dont do anything
            None
        }
    }

    pub async fn power_node(&self, node: NodeId, on: bool) -> anyhow::Result<()> {
        let mut bits = node as u8;
        if !on {
            bits = !bits;
        }

        let power_state = self.app_db.get::<u8>(POWER_STATE_KEY).await?;
        let new_power_state = Self::append_bitfield_with_mask(power_state, bits, node as u8);

        self.node_power_internal(bits, node as u8, power_state, new_power_state)
            .await?;

        self.app_db.set(POWER_STATE_KEY, new_power_state).await
    }

    pub fn append_bitfield_with_mask(current: u8, bits: u8, mask: u8) -> u8 {
        let mut result = current;
        for n in 0..size_of::<u8>() {
            // continue if bit is not to be written
            if (mask & (1 << n)) == 0 {
                continue;
            }

            // check if bit need to written high or low
            let high = (bits & (1 << n)) != 0;

            result = if high {
                result | (1 << n)
            } else {
                result & !(1 << n)
            };
        }
        result
    }

    pub async fn flash_node<P: AsRef<str>>(
        &self,
        node: NodeId,
        node_image: P,
    ) -> anyhow::Result<()> {
        todo!()
    }
}
