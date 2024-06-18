#!/bin/bash
source node_helpers.sh

# node 3 has a NVMe + 2 sata devices
assert_pci_devices "3" "1"
assert_sata_devices "3" "2"
