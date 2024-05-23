#!/bin/bash

source config.sh

eeprom=$(ls /sys/bus/i2c/devices/*/eeprom)
result=$(dd if="$eeprom" bs=1 skip=8 count=2 2>/dev/null | hexdump -e '1/1 "%02x"')

if [[ "$result" != "$ASSERT_VERSION" ]]; then
    echo "Error: expected eeprom version '$ASSERT_VERSION' got '$result'"
    exit 1
fi

model=$(< /proc/device-tree/model tr -d '\0')

if [[ "$model" != "$ASSERT_MODEL_NAME" ]]; then
    echo "Error: got '$model', expected '$ASSERT_MODEL_NAME'"
    exit 1
fi
