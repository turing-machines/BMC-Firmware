#!/bin/bash

source node_helpers.sh

assert_usb_devices "4" "4"

# NVMe drive
assert_pci_devices "4" "1"

