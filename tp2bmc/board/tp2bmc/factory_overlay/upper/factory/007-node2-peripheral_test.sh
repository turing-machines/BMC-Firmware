#!/bin/bash

source node_helpers.sh
echo -e "\tNODE2"
# node 2 has a NVMe slot + MPCIe slot
assert_pci_devices "2" "2"
echo -e "\tNODE2 => OK"
