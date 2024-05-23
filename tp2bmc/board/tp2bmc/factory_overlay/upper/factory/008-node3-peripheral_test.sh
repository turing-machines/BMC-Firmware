#!/bin/bash

source node_helpers.sh

echo -e "\tNODE3"
# node 3 has a NVMe + 2 sata devices
assert_pci_devices "3" "3"
echo -e "\tNODE3 => OK"
