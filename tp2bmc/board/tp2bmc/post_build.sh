#!/bin/bash

set -euo pipefail
cd "${0%/*}"

mkimage -A arm -T script -d boot.scr $TARGET_DIR/boot/boot.scr.uimg

if [ -e ${TARGET_DIR}/etc/inittab ]; then
	grep -qE '^GS0::' ${TARGET_DIR}/etc/inittab || \
    sed -i '/GENERIC_SERIAL/a\
GS0::respawn:/sbin/getty -L ttyGS0 115200 vt100 # BMC-USB-OTG' ${TARGET_DIR}/etc/inittab
fi
