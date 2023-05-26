use super::bits_trait::ToBits;
use crate::middleware::{
    app_persistency::ApplicationPersistency,
    event_listener::EventListener,
    pin_controller::PinController,
    usbboot::{self, FlashingError},
    NodeId, UsbMode, UsbRoute,
};
use anyhow::{ensure, Context};
use evdev::Key;
use log::debug;
use std::sync::Arc;
use tokio::sync::Mutex;

/// Stores which slots are actually used. This information is used to determine
/// for instance, which nodes need to be powered on, when such command is given
const NODE_ENABLED_KEY: &str = "node_enabled";
/// stores to which node the usb multiplexer is configured to.
const USB_NODE_KEY: &str = "usb_node";
const USB_STATE_KEY: &str = "usb_state";

#[derive(Debug)]
pub struct BmcApplication {
    pin_controller: PinController,
    app_db: ApplicationPersistency,
    power_state: Mutex<u8>,
}

impl BmcApplication {
    pub async fn new() -> anyhow::Result<Arc<Self>> {
        let pin_controller = PinController::new().await?;
        let app_db = ApplicationPersistency::new().await?;

        let instance = Arc::new(Self {
            pin_controller,
            app_db,
            power_state: Mutex::new(0),
        });

        instance.initialize().await?;

        // start listening for device events.
        EventListener::new(instance.clone())
            .add_action_async(Key::KEY_1, 1, |app| {
                Box::pin(Self::toggle_power_states(app.clone()))
            })
            .add_action_async(Key::KEY_POWER, 1, |app| {
                Box::pin(Self::toggle_power_states(app.clone()))
            })
            .add_action_async(Key::KEY_RESTART, 1, |_| {
                Box::pin(async { system_shutdown::reboot().context("reboot") })
            })
            .run()?;

        Ok(instance)
    }

    async fn toggle_power_states(app: Arc<BmcApplication>) -> anyhow::Result<()> {
        let mut lock = app.power_state.lock().await;
        *lock = !*lock;
        app.power_node(*lock).await
    }

    async fn initialize(&self) -> anyhow::Result<()> {
        self.initialize_usb_mode().await?;
        self.initialize_power().await
    }

    async fn initialize_power(&self) -> anyhow::Result<()> {
        // power on nodes
        if let Ok(enabled_nodes) = self.app_db.get::<u8>(NODE_ENABLED_KEY).await {
            self.power_node(enabled_nodes).await
        } else {
            // default, given a new app persistency
            self.app_db.set::<u8>(NODE_ENABLED_KEY, 0).await
        }
    }

    async fn initialize_usb_mode(&self) -> std::io::Result<()> {
        let node = self
            .app_db
            .get::<NodeId>(USB_NODE_KEY)
            .await
            .unwrap_or(NodeId::Node1);
        let res = self.pin_controller.select_usb(node);

        let (route, mode) = self
            .app_db
            .get::<(UsbRoute, UsbMode)>(USB_STATE_KEY)
            .await
            .unwrap_or((UsbRoute::UsbA, UsbMode::Device));
        let res2 = self.pin_controller.set_usb_route(route, mode);
        res.and(res2)
    }

    /// Helper function that returns the new state of ATX power
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

    pub async fn get_node_power(&self, node: NodeId) -> anyhow::Result<bool> {
        let state = self.power_state.lock().await;
        Ok(*state & node.to_bits() != 0)
    }

    /// This function is used to active a given node. Call this function if a
    /// module is inserted at that slot. Failing to call this method means that
    /// this slot is not considered for power up and power down commands.
    pub async fn activate_slot(&self, node: NodeId, on: bool) -> anyhow::Result<()> {
        ensure!(node.to_bits() != 0);

        let mask = node.to_bits();
        let bits = if on { node.to_bits() } else { !node.to_bits() };
        let mut state = self.app_db.get::<u8>(NODE_ENABLED_KEY).await?;
        state = (state & !mask) | (bits & mask);
        self.app_db.set::<u8>(NODE_ENABLED_KEY, state).await?;
        debug!("node enable bits updated. going to state {:#04b}", state);

        self.power_node(node).await?;
        Ok(())
    }

    pub async fn power_on(&self) -> anyhow::Result<()> {
        self.power_node(0b1111).await
    }

    pub async fn power_off(&self) -> anyhow::Result<()> {
        self.power_node(0).await
    }

    async fn power_node<N: ToBits>(&self, node: N) -> anyhow::Result<()> {
        let mask = self.app_db.get::<u8>(NODE_ENABLED_KEY).await?;
        let mut lock = self.power_state.lock().await;
        let power_state = *lock;

        let mut new_power_state = 0;
        if node.to_bits() != 0 {
            new_power_state = (power_state & !node.to_bits()) | (node.to_bits() & mask);
        }

        if power_state == new_power_state {
            debug!(
                "requested powerstate {:#04b} is already active. enabled={:#04b}",
                power_state, mask
            );
            return Ok(());
        }

        debug!(
            "applying change in power state. current state={:#04b}, new state={:#04b}",
            power_state, new_power_state
        );

        if let Some(on) = Self::need_atx_change(power_state, new_power_state) {
            debug!("changing state of atx to {}", on);
            self.pin_controller.set_atx_power(on).await?;
        }

        self.pin_controller
            .set_power_node(power_state, new_power_state)
            .await
            .context("pin controller error")?;

        *lock = new_power_state;
        Ok(())
    }

    pub async fn usb_mode(&self, mode: UsbMode, node: NodeId) -> anyhow::Result<()> {
        self.pin_controller.select_usb(node)?;
        self.app_db.set(USB_NODE_KEY, node).await?;
        self.pin_controller.set_usb_route(UsbRoute::BMC, mode)?;
        self.app_db.set(USB_STATE_KEY, (UsbRoute::BMC, mode)).await
    }

    pub async fn rtl_reset(&self) -> anyhow::Result<()> {
        self.pin_controller.rtl_reset().await.context("rtl error")
    }

    pub async fn flash_node<P: AsRef<str>>(
        &self,
        node: NodeId,
        _image_path: P,
    ) -> Result<(), FlashingError> {
        usbboot::boot_node_to_msd(node)
    }
}
