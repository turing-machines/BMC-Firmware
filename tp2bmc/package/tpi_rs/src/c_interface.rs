//! This module acts as a glue layer for the legacy bmc application. It exports
//! relevant API functions over FFI. This FFI interface is temporary and will be
//! removed as soon as the bmc application is end of life.

use anyhow::Context;
use futures::future::BoxFuture;
use futures::Future;
use log::{error, LevelFilter};
use once_cell::sync::{Lazy, OnceCell};
use simple_logger::SimpleLogger;
use std::sync::Arc;
use std::{ffi::CStr, ops::Deref};
use tokio::{runtime::Runtime, sync::Mutex};

use crate::middleware::usbboot::FlashingError;
use crate::{app::bmc_application::BmcApplication, middleware::NodeId};

/// we need means to synchronize async call to the outside. This runtime
/// enables us to execute async calls in a blocking fashion.
static RUNTIME: Lazy<Runtime> =
    Lazy::new(|| Runtime::new().expect("fatal error initializing runtime"));
static APP: OnceCell<Mutex<Arc<BmcApplication>>> = OnceCell::new();

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

fn execute_routine<F>(func: F)
where
    F: for<'a> Fn(&'a BmcApplication) -> BoxFuture<'a, anyhow::Result<()>>,
{
    RUNTIME.block_on(async move {
        let lock = APP.get().unwrap().lock();
        if let Err(e) = func(lock.await.deref()).await {
            error!("{}", e);
        }
    })
}

#[no_mangle]
pub extern "C" fn node_power(num: core::ffi::c_int, status: core::ffi::c_int) {
    let Ok(node_id) = num.try_into().map_err(|e| log::error!("{}", e)) else {
        return;
    };

    execute_routine(|bmc| Box::pin(bmc.power_node(node_id, status != 0)))
}

#[no_mangle]
pub extern "C" fn poweron() {
    execute_routine(|bmc| Box::pin(bmc.power_node(NodeId::All, true)))
}

#[no_mangle]
pub extern "C" fn poweroff() {
    execute_routine(|bmc| Box::pin(bmc.power_node(NodeId::All, false)))
}

#[no_mangle]
pub extern "C" fn ctrl_usbconnet(
    mode: core::ffi::c_int,
    node: core::ffi::c_int,
) -> core::ffi::c_int {
    let Ok(node_id) = node.try_into().map_err(|e| log::error!("{}", e)) else {
        return -1;
    };
    let Ok(mode) = mode.try_into().map_err(|e| log::error!("{}", e)) else {
        return -1;
    };
    execute_routine(|bmc| {
        Box::pin(async move { bmc.usb_mode(mode, node_id).context("usb_mode error") })
    });
    0
}

#[no_mangle]
pub extern "C" fn get_node_power(node: core::ffi::c_int) -> core::ffi::c_int {
    let Ok(node_id) = node.try_into().map_err(|e| log::error!("{}", e)) else {
        return -1;
    };

    RUNTIME.block_on(async move {
        let lock = APP.get().unwrap().lock();
        if lock.await.deref().get_node_power(node_id).await.unwrap() {
            1
        } else {
            0
        }
    })
}

#[no_mangle]
pub extern "C" fn RTL_Reset() {
    todo!()
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
pub extern "C" fn tpi_flash_node(
    node: core::ffi::c_int,
    image_path: *const core::ffi::c_char,
) -> FlashingResult {
    let str = unsafe { CStr::from_ptr(image_path) }.to_str().unwrap();
    let node_image = String::from(str);

    let Ok(node_id) = node.try_into() else {
        return FlashingResult::InvalidArgs
    };

    log::debug!("using file '{}'", node_image);

    RUNTIME.block_on(async move {
        let lock = APP.get().unwrap().lock();
        lock.await
            .deref()
            .flash_node(node_id, node_image)
            .await
            .into()
    })
}
