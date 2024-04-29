#!/bin/bash

set -euo pipefail
BOARD_DIR="${0%/*}"
cd $BINARIES_DIR

# Prepare the installer initramfs image
INITRAMFS_DIR=$STAGING_DIR/initramfs/install
(cd $INITRAMFS_DIR && find .) |\
	cpio -oH newc -D $INITRAMFS_DIR |\
	gzip > installer.cpio.gz

# Prepare the SD image's FAT32 filesystem
mkdir -p sdcard-bootpart/boot
mkimage -A arm -T script -d $BOARD_DIR/install.scr sdcard-bootpart/boot/boot.scr.uimg
mkimage -A arm -T ramdisk -d installer.cpio.gz sdcard-bootpart/boot/install.img
cp -r $BOARD_DIR/sdcard_overlay/* sdcard-bootpart/
cp zImage *.dtb sdcard-bootpart/boot/

# Generate the SD image
[ -d tmp/ ] && rm -fr tmp/
genimage --inputpath . --outputpath . --rootpath sdcard-bootpart \
	--config $BOARD_DIR/genimage.cfg
