#!/bin/bash

source node_helpers.sh

# NVMe drive
assert_pci_devices "4" "1"
return_code1=$?
assert_usb_devices "4" "4"
exit $(( $return_code1+2*$? ))
