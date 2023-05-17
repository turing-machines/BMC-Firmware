//! This module acts as a glue layer for the legacy bmc application. It exports
//! relevant API functions over FFI. This FFI interface is temporary and will be
//! removed as soon as the bmc application is end of life.

use log::LevelFilter;
use once_cell::sync::{Lazy, OnceCell};
use simple_logger::SimpleLogger;
use std::{ffi::CStr, ops::Deref};
use tokio::{runtime::Runtime, sync::Mutex};

use crate::{app::bmc_application::BmcApplication, middleware::NodeId};
use crate::middleware::usbboot::FlashingError;

/// we need means to synchronize async call to the outside. This runtime
/// enables us to execute async calls in a blocking fashion.
static RUNTIME: Lazy<Runtime> =
    Lazy::new(|| Runtime::new().expect("fatal error initializing runtime"));
static APP: OnceCell<Mutex<BmcApplication>> = OnceCell::new();

#[no_mangle]
pub extern "C" fn tpi_initialize() {
    RUNTIME.block_on(async move {
        let level = if cfg!(debug_assertions) {
            LevelFilter::Debug
        } else {
            LevelFilter::Warn
        };

        SimpleLogger::new()
            .with_level(level)
            .with_colors(true)
            .env()
            .init()
            .expect("failed to initialize logger");

        log::info!("Turing Pi BMC firmware v{}", env!("CARGO_PKG_VERSION"));

        APP.set(Mutex::new(
            BmcApplication::new().await.expect("unable to initialize"),
        ))
        .expect("initialize to be called once");
    });
}

fn power_cycle_routine(node_id: NodeId, on: bool) {
    RUNTIME.block_on(async move {
        let lock = APP.get().unwrap().lock();
        lock.await.deref().power_node(node_id, on).await.unwrap();
    });
}
/// Turns a given node on or off.
///
/// # Arguments
///
/// 'node_id'   specify which node will be effected
/// 'on'        when on==true, power on, on==false : power down.
#[no_mangle]
pub extern "C" fn power_cycle_node(node_id: NodeId, on: bool) {
    power_cycle_routine(node_id, on);
}

#[no_mangle]
pub extern "C" fn power_system_on() {
    power_cycle_routine(NodeId::All, true);
}

#[no_mangle]
pub extern "C" fn power_system_off() {
    power_cycle_routine(NodeId::All, false);
}

#[allow(dead_code)]
#[repr(C)]
pub enum FlashingResult {
    Success,
    InvalidArgs,
    Timeout,
    ChecksumMismatch,
}

impl From<Result<(), FlashingError>> for FlashingResult {
    fn from(value: Result<(), FlashingError>) -> Self {
        match value {
            Ok(_) => FlashingResult::Success,
            Err(FlashingError::InvalidArgs) => FlashingResult::InvalidArgs,
            Err(FlashingError::Timeout) => FlashingResult::Timeout,
            Err(FlashingError::ChecksumMismatch) => FlashingResult::ChecksumMismatch,
        }
    }
}

#[no_mangle]
pub extern "C" fn tpi_flash_node(node: u32, image_path: *const std::ffi::c_char) -> FlashingResult {
    let str = unsafe { CStr::from_ptr(image_path) }.to_str().unwrap();
    let node_image = String::from(str);

    let node_id = match node.try_into() {
        Ok(value) => value,
        Err(_) => return FlashingResult::InvalidArgs,
    };

    RUNTIME.block_on(async move {
        let lock = APP.get().unwrap().lock();
        lock.await
            .deref()
            .flash_node(node_id, node_image)
            .await
            .into()
    })
}
