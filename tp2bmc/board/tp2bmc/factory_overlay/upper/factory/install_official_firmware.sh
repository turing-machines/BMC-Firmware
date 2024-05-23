#!/bin/bash

read -p "Enter the the last digits of product serial from the sticker: ${PRODUCT_SERIAL_PREFIX}${PRODUCTION_TIME}" serial

./install_firmware
if [[ $? -ne 0 ]]; then
    echo -e "${RED} installation of firmware failed!${NC}"
    exit 1
fi

# insert cookie to be picked up by bootloader.
# This signals the bootloader not load the factory image, but too boot from
# flash
install_magic=0x12341234
install_addr=0x0709010C
devmem  $install_addr 32 $install_magic


days_since_may() {
# Extract year and month from PRODUCTION_TIME
local year="20${PRODUCTION_TIME:0:2}"
local month="${PRODUCTION_TIME:2:2}"

# target date: may 2024
local target_year=2024
local target_month=5

# convert both dates to seconds since unix epoch
local production_date_seconds=$(date -d "${year}-${month}-01" +%s)
local target_date_seconds=$(date -d "${target_year}-${target_month}-01" +%s)

# calculate the difference in seconds and then convert to days
local diff_seconds=$((production_date_seconds - target_date_seconds))
local diff_days=$((diff_seconds / 86400))
echo "$diff_days"
}


tpi_factory_serial="${PRODUCT_SERIAL_PREFIX}${serial}" \
tpi_product_name="$PRODUCT_NAME" \
tpi_production_time="$(days_since_may)" \
tpi eeprom set

if [[ $? -ne 0 ]]; then
    echo -e "${RED} Error burning eeprom!${NC}"
    exit 1
fi
