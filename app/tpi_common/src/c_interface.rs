//! This module acts as a glue layer for the legacy bmc application. It exports
//! relevant API functions over FFI. This FFI interface is temporary and will be
//! removed as soon as the bmc application is end of life.

use crate::{
    power::{power_off, power_on},
    NodeId,
};
use once_cell::sync::Lazy;
use tokio::runtime::Runtime;

/// we need means to synchronize async call to the outside. This runtime
/// enables us to execute async calls in a blocking fashion.
static RUNTIME: Lazy<Runtime> =
    Lazy::new(|| Runtime::new().expect("fatal error initializing runtime"));

/// Turns a given node on or off.
///
/// # Arguments
///
/// 'node_id'   specify which node will be effected
/// 'on'        when on==true, power on, on==false : power down.
#[no_mangle]
pub extern "C" fn power_cycle_node(node_id: NodeId, on: bool) -> i32 {
    RUNTIME.block_on(async move {
        let res = if on {
            power_on(node_id).await
        } else {
            power_off(node_id).await
        };

        //TODO: if res not ok, handle error
    });
    0
}

#[repr(C)]
pub enum NodeFlashProgress {
    UnknownError,
    NodeDoesNotExist,
    /// The node is already undergoing a fwupdate. Fw update request is ignored
    Busy,
    /// Flashing is in progress, node does not respond and any subsequent fw
    /// update requests are ignored.
    InProgress,
    /// Cannot successfully progress the fw udate due to unsufficient storage
    /// space on the host.
    StorageFull,
    /// The node does not respond anymore
    Timeout,
    /// verification step after fw update failed. Upload may be corrupted
    ChecksumMismatch,
    /// The fw update completed successfully.
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
    image_path: *mut core::ffi::c_char,
    progress_cb: FlashProgressFn,
) {
    RUNTIME.block_on(async move {
        todo!();
    });
}
