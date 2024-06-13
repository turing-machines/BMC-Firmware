#!/bin/bash
source node_helpers.sh

result=$(send_command "1" "lsusb -d ${BMC_OTG}")
if [[ $? -ne 0 ]]; then
    echo "Error: NODE1_USB_HOST or BMC_USB_OTG port not responding"
    echo "Verify if cable is attached between the 2 USB ports. The BMC should expose an USB device with ID ${BMC_OTG} over the BMC_USB_OTG port"
    exit 1
fi

