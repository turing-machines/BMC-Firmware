//! This module acts as a glue layer for the legacy bmc application. It exports
//! relevant API functions over FFI. This FFI interface is temporary and will be
//! removed as soon as the bmc application is end of life.
use crate::{app::bmc_application::BmcApplication, middleware::NodeId};
use once_cell::sync::{Lazy, OnceCell};
use std::{ffi::CStr, ops::Deref};
use tokio::{runtime::Runtime, sync::Mutex};

/// we need means to synchronize async call to the outside. This runtime
/// enables us to execute async calls in a blocking fashion.
static RUNTIME: Lazy<Runtime> =
    Lazy::new(|| Runtime::new().expect("fatal error initializing runtime"));
static APP: OnceCell<Mutex<BmcApplication>> = OnceCell::new();

#[no_mangle]
pub extern "C" fn initialize() {
    RUNTIME.block_on(async move {
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

#[repr(C)]
pub enum NodeFlashProgress {
    UnknownError,
    //    NodeDoesNotExist,
    //    /// The node is already undergoing a fwupdate. Fw update request is ignored
    //    Busy,
    //    /// Flashing is in progress, node does not respond and any subsequent fw
    //    /// update requests are ignored.
    //    InProgress,
    //    /// Cannot successfully progress the fw udate due to unsufficient storage
    //    /// space on the host.
    //    StorageFull,
    //    /// The node does not respond anymore
    //    Timeout,
    //    /// verification step after fw update failed. Upload may be corrupted
    //    ChecksumMismatch,
    //    /// The fw update completed successfully.
    Done,
}

/// C callback that reports the progress of an running firmware update to one of
/// the connected compute modules.
///
/// # Arguments
///
/// * 'status_code'   reports the state of the running firmware update. A status
/// code [NodeFlashProgress::Done] is the end of the sequence and signals a
/// successful update
///
/// * 'progress' reports the progress of the firmware update. Ranges from zero to
/// [u32::Max], where [u32::Max] == 100%.
type FlashProgressFn = extern "C" fn(status_code: NodeFlashProgress, progress: u32);

#[no_mangle]
pub extern "C" fn flash_node(
    nodeId: NodeId,
    image_path: *mut std::os::raw::c_char,
    progress_cb: FlashProgressFn,
) {
    let str = unsafe { CStr::from_ptr(image_path) }.to_str().unwrap();
    let node_image = String::from(str);

    RUNTIME.block_on(async move {
        let lock = APP.get().unwrap().lock();
        lock.await
            .deref()
            .flash_node(nodeId, node_image)
            .await
            .unwrap();
    });
}
