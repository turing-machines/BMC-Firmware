#!/bin/bash
source node_helpers.sh

# node 3 has a NVMe + 2 sata devices
assert_pci_devices "3" "1"
return_code1=$?
assert_sata_devices "3" "2"
exit $(( $return_code1+2*$? ))
