#!/bin/bash

source node_helpers.sh
# node 2 has a NVMe slot + MPCIe slot
assert_pci_devices "2" "2"
