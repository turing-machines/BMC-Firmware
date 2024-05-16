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

# generate initramfs purposed for FEL upgrades
FEL_INITRAMFS_DIR=$STAGING_DIR/usb_initramfs
(cd $FEL_INITRAMFS_DIR && find .) |\
cpio -oH newc -D $FEL_INITRAMFS_DIR |\
gzip > usb_installer.cpio.gz

mkimage -A arm -T ramdisk -d usb_installer.cpio.gz usb_initramfs.img
mkimage -A arm -T script -d $BOARD_DIR/ram_runner.scr ram_runner.scr.uimg
