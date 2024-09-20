#!/bin/bash

source node_helpers.sh

#tpi advanced msd --node 2 >/dev/null
#tpi usb device --node 2 > /dev/null

return_code=1

for i in {1..5}; do
    devices=$(get_usb_devices "1")
    if echo "$devices" | grep -q "ID ${RK1_USB}"; then
        #echo -e "\tUSB_DEV => OK"
        return_code=0
        break
    fi
    #sleep 0.1
    tpi advanced msd --node 2 >/dev/null
    tpi usb device --node 2 > /dev/null
    sleep 1
done

if [ $return_code -eq 1 ]; then
    echo "$devices"
    echo "Error: Could not detect 'USB_DEV' port"
    exit 1
fi

echo -e "\tUSB_DEV => OK"
