#!/bin/bash

source node_helpers.sh

echo -e "\tNODE1"
result=$(send_command "1" "lsusb -d ${BMC_OTG}")
if [[ $? -ne 0 ]]; then
    echo -e "lsusb node 1:\n$(send_command "1" "lsusb ")"
    echo "Error: NODE1_USB_HOST or BMC_USB_OTG port not responding"
    exit 1
fi
confirm "Do you see video output on the HDMI port?"
confirm "Do you see video output on the DSI port?"
# node 1 has a NVMe slot + MPCIe slot
assert_pci_devices "1" "2"
echo -e "\tNODE1 => OK"
