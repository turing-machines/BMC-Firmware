#!/bin/bash

set -e
eeprom=$(ls /sys/bus/i2c/devices/*/eeprom)
dd if=/dev/zero bs=1 count=50 | tr '\0' '\377' > "$eeprom"
echo "EEPROM erased successfully"

