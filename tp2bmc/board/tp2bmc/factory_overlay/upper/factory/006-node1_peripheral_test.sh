#!/bin/bash
source node_helpers.sh

# node 1 has a NVMe slot + MPCIe slot
assert_pci_devices "1" "2"
exit $?
