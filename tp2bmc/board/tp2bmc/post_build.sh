#!/bin/bash

set -euo pipefail
# buildroots uboot-tools are ancient. Use the one from our uboot build.
mkimage="$BUILD_DIR/uboot-*/tools/mkimage"

cd "${0%/*}"

$mkimage -A arm -T script -d boot.scr $TARGET_DIR/boot/boot.scr.uimg

cp $PWD/*.its "$BINARIES_DIR/"
cd "$BINARIES_DIR"
$mkimage -E -f "turing-pi2.its" "$TARGET_DIR/boot/turing-pi2.itb"

if [ -e ${TARGET_DIR}/etc/inittab ]; then
	grep -qE '^GS0::' ${TARGET_DIR}/etc/inittab || \
    sed -i '/GENERIC_SERIAL/a\
GS0::respawn:/sbin/getty -L ttyGS0 115200 vt100 # BMC-USB-OTG' ${TARGET_DIR}/etc/inittab
fi
