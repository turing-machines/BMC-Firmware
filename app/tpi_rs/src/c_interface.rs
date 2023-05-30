//! This module acts as a glue layer for the legacy bmc application. It exports
//! relevant API functions over FFI. This FFI interface is temporary and will be
//! removed as soon as the bmc application is end of life.
use futures::future::BoxFuture;
use log::{error, LevelFilter};
use once_cell::sync::{Lazy, OnceCell};
use simple_logger::SimpleLogger;
use std::path::PathBuf;
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
            LevelFilter::Trace
        } else {
            LevelFilter::Info
        };

        SimpleLogger::new()
            .with_level(level)
            .with_module_level("sqlx", LevelFilter::Warn)
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
pub extern "C" fn tpi_node_power(num: core::ffi::c_int, status: core::ffi::c_int) {
    let Ok(node_id): Result<NodeId,()> = num.try_into().map_err(|e| log::error!("{}", e)) else {
        return;
    };

    execute_routine(|bmc| Box::pin(bmc.activate_slot(node_id, status != 0)))
}

#[no_mangle]
pub extern "C" fn tpi_power_on() {
    execute_routine(|bmc| Box::pin(bmc.power_on()))
}

#[no_mangle]
pub extern "C" fn tpi_power_off() {
    execute_routine(|bmc| Box::pin(bmc.power_off()))
}

#[no_mangle]
pub extern "C" fn tpi_usb_mode(mode: core::ffi::c_int, node: core::ffi::c_int) -> core::ffi::c_int {
    let Ok(node_id) = node.try_into().map_err(|e| log::error!("{}", e)) else {
        return -1;
    };
    let Ok(mode) = mode.try_into().map_err(|e| log::error!("{}", e)) else {
        return -1;
    };
    execute_routine(|bmc| Box::pin(bmc.usb_mode(mode, node_id)));
    0
}

#[no_mangle]
pub extern "C" fn tpi_get_node_power(node: core::ffi::c_int) -> core::ffi::c_int {
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
pub extern "C" fn tpi_rtl_reset() {
    execute_routine(|bmc| Box::pin(bmc.rtl_reset()))
}

#[allow(dead_code)]
#[repr(C)]
pub enum FlashingResult {
    Success,
    InvalidArgs,
    DeviceNotFound,
    GpioError,
    UsbError,
    IoError,
    Timeout,
    ChecksumMismatch,
    Other,
}

impl From<&FlashingError> for FlashingResult {
    fn from(value: &FlashingError) -> Self {
        match value {
            FlashingError::InvalidArgs => FlashingResult::InvalidArgs,
            FlashingError::DeviceNotFound => FlashingResult::DeviceNotFound,
            FlashingError::GpioError => FlashingResult::GpioError,
            FlashingError::UsbError => FlashingResult::UsbError,
            FlashingError::IoError => FlashingResult::IoError,
            FlashingError::Timeout => FlashingResult::Timeout,
            FlashingError::ChecksumMismatch => FlashingResult::ChecksumMismatch,
        }
    }
}

#[no_mangle]
pub extern "C" fn tpi_flash_node(
    node: core::ffi::c_int,
    image_path: *const core::ffi::c_char,
) -> FlashingResult {
    let cstr = unsafe { CStr::from_ptr(image_path) };
    let Ok(bstr) = cstr.to_str() else {
        return FlashingResult::InvalidArgs;
    };
    let node_image = PathBuf::from(bstr);

    let Ok(node_id) = node.try_into() else {
        return FlashingResult::InvalidArgs
    };

    RUNTIME.block_on(async move {
        let lock = APP.get().unwrap().lock();
        let res = lock.await.deref().flash_node(node_id, node_image).await;

        match res {
            Ok(_) => FlashingResult::Success,
            Err(cause) => {
                if let Some(flashing_err) = cause.downcast_ref::<FlashingError>() {
                    flashing_err.into()
                } else {
                    FlashingResult::Other
                }
            }
        }
    })
}
