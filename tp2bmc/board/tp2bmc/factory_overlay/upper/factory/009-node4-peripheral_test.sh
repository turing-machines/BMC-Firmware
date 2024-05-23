#!/bin/bash

source node_helpers.sh

echo -e "\tNODE4"
# 2 at the back and 2 at the front panel
usb_count=4
usb_devices=$(get_usb_devices 4)
line_count=$(echo "$usb_devices" | wc -l)
if [[ "$line_count" -lt "$usb_count" ]]; then
    echo "Error: Fewer than ${usb_count}(${line_count}) USB devices connected to NODE4"
    echo "got: ${usb_devices}"
    exit 1
fi

echo -e "\tUSB3X2 NODE4"
print_usb_names "$usb_devices"
echo -e "\tUSB3X2 NODE4 => OK"

# NVMe drive
assert_pci_devices "4" "1"
echo -e "\tNODE4 => OK"

