use crate::middleware::{
    app_persistency::ApplicationPersistency,
    evdev_controller::linux_event_stream,
    pin_controller::PinController,
    usbboot::{self, FlashingError},
    NodeId, UsbMode, UsbRoute,
};
use anyhow::Context;
use log::{debug, warn};
use std::sync::Arc;

const POWER_STATE_KEY: &str = "power_state";
const USB_MODE_KEY: &str = "usb_mode_state";
const USB_ROUTE_KEY: &str = "usb_route_state";
const USB_NODE_KEY: &str = "usb_node_state";

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
        Self::run_event_listener(instance.clone());
        Ok(instance)
    }

    async fn initialize(&self) -> anyhow::Result<()> {
        self.initialize_power().await?;
        self.initialize_usb_mode().await
    }

    async fn initialize_power(&self) -> anyhow::Result<()> {
        // power on nodes
        if let Ok(power_state) = self.app_db.get::<u8>(POWER_STATE_KEY).await {
            self.node_power_internal(0, power_state).await
        } else {
            self.app_db.set::<u8>(POWER_STATE_KEY, 0).await
        }
    }

    async fn initialize_usb_mode(&self) -> anyhow::Result<()> {
        self.usb_mode(UsbMode::Slave, NodeId::Node1)?;
        Ok(())
    }

    /// A small controller that listens for the KEY_1 press and acts
    /// accordingly.
    fn run_event_listener(app: Arc<BmcApplication>) {
        tokio::spawn(async move {
            let mut event_stream = linux_event_stream().unwrap();
            let mut state = false;
            while let Ok(event) = event_stream.next_event().await {
                match (event.kind(), event.value()) {
                    (evdev::InputEventKind::Key(evdev::Key::KEY_1), 1) => {
                        state = !state;
                        app.power_node(NodeId::All, state).await.unwrap();
                    }
                    _ => debug!("event omitted {:?}", event),
                }
            }
        });
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
        Ok(state & node.to_bitfield() != 0)
    }

    /// Power on or off a given node. Nodes are switched with a small delay to
    /// protect the hardware. The state of the nodes are persisted to disk.
    pub async fn power_node(&self, node: NodeId, on: bool) -> anyhow::Result<()> {
        let mut bits = node.to_bitfield();
        if !on {
            bits = !bits;
        }
        let mask = node.to_bitfield();

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

    pub fn usb_mode(&self, mode: UsbMode, node: NodeId) -> std::io::Result<()> {
        self.pin_controller.select_usb(node)?;
        self.pin_controller.set_usb_route(UsbRoute::BMC, mode)
    }

    pub async fn flash_node<P: AsRef<str>>(
        &self,
        node: NodeId,
        _image_path: P,
    ) -> Result<(), FlashingError> {
        usbboot::boot_node_to_msd(node)
    }
}
