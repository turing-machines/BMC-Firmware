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

void tpi_initialize(void);

void power_cycle_node(node_id_t node_id, bool on);

typedef enum {
    FR_SUCCESS,
    FR_INVALID_ARGS,
    FR_DEVICE_NOT_FOUND,
    FR_GPIO_ERROR,
    FR_USB_ERROR,
    FR_IO_ERROR,
    FR_TIMEOUT,
    FR_CHECKSUM_MISMATCH,
    FR_OTHER,
} flashing_result;

flashing_result tpi_flash_node(uint32_t node_id, const char* image_path);

