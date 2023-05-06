// this header file corresponds to the c_interface module. Refer to
// c_interface.rs for more detailed function description.
#pragma once
#include <stdint.h>

typedef enum {
    Node1,
    Node2,
    Node3,
    Node4,
    All,
} node_id_t;

void power_cycle_node(node_id_t node_id, bool on);

typedef enum  {
    UnknownError,
    NodeDoesNotExist,
    Busy,
    InProgress,
    StorageFull,
    Timeout,
    ChecksumMismatch,
    Done,
} node_flash_progress_t;

void flash_node(node_id_t node_id, void(*on_progress)(node_flash_progress_t, uint32_t));

