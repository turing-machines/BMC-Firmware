#!/bin/bash

source config.sh
echo -e "\n"
echo -e "${YELLOW}Verify now if:${NC}"
echo -e "${YELLOW}  - the fan is spinning${NC}"
echo -e "${YELLOW}  - there is picture on the monitor${NC}"
confirm "Confirm verification"
echo -e "\n"
read -p "Enter product serial from the sticker: " serial

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

generate_mac() {
    # its been told that the factory serial is composed out of the following
    # elements:
    # XZC:Our company name initial
    # T: project name of Turing PI initial
    # 2211:Production time November 2022
    # 00605:Product serial number

    # get the last five digits
    local product_digits=$((10#${serial: -5}))
    if ! [[ "$product_digits" =~ ^[0-9]+$ ]]; then
        echo -e "${RED} ERROR: provided serial is not correct. Expected a serial ending with at least containing 5 digits${NC}"
        exit 1
    fi

    eui_48=$(printf "%05X" "$product_digits")
    echo "${TPI_MAC_BASE}${eui_48}"
}

generated_mac=$(generate_mac)

tpi_factory_serial="${serial}" \
tpi_product_name="$PRODUCT_NAME" \
tpi_production_time="$(days_since_may)" \
tpi_mac="$generated_mac" \
tpi eeprom set

if [[ $? -ne 0 ]]; then
    echo -e "${RED} Error burning eeprom!${NC}"
    exit 1
fi

echo "Running post test script.."
./post_test.sh &

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
