#!/bin/bash
source node_helpers.sh

return_code=1
for i in {1..20}; do
    devices=$(get_usb_devices "1")
    if echo "$devices" | grep -q "ID ${BMC_OTG}"; then
        return_code=0
        break
    fi
    sleep 0.5
done
if [ $return_code -eq 1 ]; then
    echo "Error: NODE1_USB_HOST or BMC_USB_OTG port not responding"
    echo "Verify if cable is attached between the 2 USB ports. The BMC should expose an USB device with ID ${BMC_OTG} over the BMC_USB_OTG port"
    exit 1
fi

echo -e "\tUSB_HOST => OK"
