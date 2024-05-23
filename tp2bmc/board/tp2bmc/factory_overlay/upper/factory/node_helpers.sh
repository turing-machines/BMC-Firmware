#!/bin/bash

source helpers.sh
BMC_OTG="08DE:1234"
RK1_USB="2207:350b"

assert_pci_devices() {
    local devices=$(get_pci_devices "$1" | grep -v '^$')
    local count="$2"
    line_count=$(echo "$devices" | wc -l)
    # Exit with an error if there are fewer than 2 lines
    if [ "$line_count" -lt "${count}" ]; then
        echo "Error: Fewer than ${count} (got ${line_count}) PCI devices connected "
        echo -e "\tFound:"
        echo -e "\t${devices}"
        echo "Error: Node ${n}: could not detect NVMe or MPCIE device"
        exit 1
    fi
}

print_usb_names() {
    local usb_devices="$1"
    # Loop through each line and print the name of the USB device
    echo "$usb_devices" | while IFS= read -r line; do
    # Extract and print the device name (fields starting from 7th position)
    local dev=$(echo "$line" | awk '{for(i=7;i<=NF;i++) printf $i" "; print ""}')
    echo -e "\t\t${dev}"
    done
}

usb_dev_test() {
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
}

