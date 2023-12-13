#!/bin/bash

set -euo pipefail
cd "${0%/*}"

FDT_FILENAME=$(basename $TARGET_DIR/boot/*.dtb)
sed s/FDT_FILENAME_HERE/${FDT_FILENAME}/g < boot.scr > /tmp/boot.scr
mkimage -A arm -T script -d /tmp/boot.scr $TARGET_DIR/boot/boot.scr.uimg

if [ -e ${TARGET_DIR}/etc/inittab ]; then
	grep -qE '^GS0::' ${TARGET_DIR}/etc/inittab || \
    sed -i '/GENERIC_SERIAL/a\
GS0::respawn:/sbin/getty -L ttyGS0 115200 vt100 # BMC-USB-OTG' ${TARGET_DIR}/etc/inittab
fi
