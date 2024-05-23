#!/bin/bash

source node_helpers.sh

tpi advanced msd --node 2 >/dev/null
tpi usb device --node 2 > /dev/null
confirm "plug the USB cable from 'BMC_USB_OTG' to the 'USB_DEV' port"
devices=$(get_usb_devices "1")
if echo "$devices" | grep -q "ID ${RK1_USB}"; then
    echo -e "\tUSB_DEV => OK"
else
    echo "$devices"
    echo "Error: Could not detect 'USB_DEV' port"
    exit 1
fi
