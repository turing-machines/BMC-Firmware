#!/bin/bash

set -euo pipefail
BOARD_DIR="${0%/*}"
cd $BINARIES_DIR

# Prepare the SD image's FAT32 filesystem
mkdir -p sdcard-bootpart/boot
FDT_FILENAME=$(basename $TARGET_DIR/boot/*.dtb)
sed s/FDT_FILENAME_HERE/${FDT_FILENAME}/g < $BOARD_DIR/install.scr > /tmp/install.scr
mkimage -A arm -T script -d /tmp/install.scr sdcard-bootpart/boot/boot.scr.uimg
cp -r $BOARD_DIR/sdcard_overlay/* sdcard-bootpart/

# Generate the SD image
[ -d tmp/ ] && rm -fr tmp/
genimage --inputpath . --outputpath . --rootpath sdcard-bootpart \
	--config $BOARD_DIR/genimage.cfg
