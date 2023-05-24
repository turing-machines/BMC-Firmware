use super::bits_trait::ToBits;
use crate::middleware::{
    app_persistency::ApplicationPersistency,
    evdev_controller::EventListener,
    pin_controller::PinController,
    usbboot::{self, FlashingError},
    NodeId, UsbMode, UsbRoute,
};
use anyhow::Context;
use evdev::Key;
use log::debug;
use std::sync::Arc;

const POWER_STATE_KEY: &str = "power_state";
const USB_NODE_KEY: &str = "usb_node";
const USB_STATE_KEY: &str = "usb_state";

#[derive(Debug)]
pub struct BmcApplication {
    pin_controller: PinController,
    app_db: ApplicationPersistency,
}

impl BmcApplication {
    pub async fn new() -> anyhow::Result<Arc<Self>> {
        let pin_controller = PinController::new().await.context("pin controller")?;
        let app_db = ApplicationPersistency::new().await.context("persistency")?;

        let instance = Arc::new(Self {
            pin_controller,
            app_db,
        });

        instance.initialize().await?;

        EventListener::new(instance.clone())
            .add_action_async(Key::KEY_1, 1, |app| {
                Box::pin(Self::toggle_power_states(app.clone()))
            })
            .add_action_async(Key::KEY_POWER, 1, |app| todo!())
            .add_action_async(Key::KEY_RESTART, 1, |app| todo!())
            .run()?;

        Ok(instance)
    }

    async fn toggle_power_states(app: Arc<BmcApplication>) -> anyhow::Result<()> {
        let state = app.app_db.get::<u8>(POWER_STATE_KEY).await?;
        // calling node power with on == false, inverts the passed state bits.
        app.power_node(state, false).await
    }

    async fn initialize(&self) -> anyhow::Result<()> {
        self.initialize_usb_mode().await?;
        self.initialize_power().await
    }

    async fn initialize_power(&self) -> anyhow::Result<()> {
        // power on nodes
        if let Ok(power_state) = self.app_db.get::<u8>(POWER_STATE_KEY).await {
            self.node_power_internal(0, power_state).await
        } else {
            self.app_db.set::<u8>(POWER_STATE_KEY, 0).await
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
            .unwrap_or((UsbRoute::UsbA, UsbMode::Slave));
        let res2 = self.pin_controller.set_usb_route(route, mode);
        res.and(res2)
    }

    /// Helper function that calls power function of the nodes while checking
    /// if atx power is in the correct state.
    async fn node_power_internal(
        &self,
        current_node_state: u8,
        new_node_state: u8,
    ) -> anyhow::Result<()> {
        if let Some(on) = Self::need_atx_change(current_node_state, new_node_state) {
            debug!("changing state of atx to {}", on);
            self.pin_controller.set_atx_power(on).await?;
        }

        self.pin_controller
            .set_power_node(current_node_state, new_node_state)
            .await
            .context("pin controller error")
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
        let state = self.app_db.get::<u8>(POWER_STATE_KEY).await?;
        Ok(state & node.to_bits() != 0)
    }

    /// Power on or off a given node. Nodes are switched with a small delay to
    /// protect the hardware. The state of the nodes are persisted to disk.
    pub async fn power_node<N: ToBits>(&self, node: N, on: bool) -> anyhow::Result<()> {
        let mut bits = node.to_bits();
        if !on {
            bits = !bits;
        }
        let mask = node.to_bits();
        let power_state = self.app_db.get::<u8>(POWER_STATE_KEY).await?;
        let new_power_state = (power_state & !mask) | (bits & mask);

        debug!(
            "power_node {:#04b} {}. current state={:#08b}, new state={:#08b}",
            bits,
            if on { "on" } else { "off" },
            power_state,
            new_power_state
        );

        self.node_power_internal(power_state, new_power_state)
            .await?;

        self.app_db.set(POWER_STATE_KEY, new_power_state).await
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
