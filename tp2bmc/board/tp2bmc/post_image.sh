#!/bin/bash

set -euo pipefail
BOARD_DIR="${0%/*}"
cd $BINARIES_DIR

create_sdcard() {
    rootpath=$(mktemp -d)
    gencfg="$1"
    bootscript="$2"

    mkdir -p "$rootpath"/boot
    mkimage -A arm -T script -d "$bootscript" "$rootpath"/boot/boot.scr.uimg
    mkimage -A arm -T ramdisk -d installer.cpio.gz "$rootpath"/boot/install.img
    cp -r $BOARD_DIR/sdcard_overlay/* "$rootpath"/

    chmod -R 755 "$rootpath"

    # Generate the SD image
    [ -d tmp/ ] && rm -fr tmp/
    genimage --inputpath . --outputpath . --rootpath "$rootpath" \
        --config "$gencfg"

    rm -rf "$rootpath"
}

factory_sdcard() {
    # prepare factory overlay partition
    cp -r "$BOARD_DIR"/factory_overlay .

    # Git doesnt support c nodes, manually create them from a .txt file
    while IFS= read -r line
    do
        dest="factory_overlay/upper/${line}"
        echo "deleting ${line} in factory overlay"
        rm -f "$dest"
        mknod  "$dest" c 0 0
        chmod 000 "$dest"
    done < "factory_overlay/files_to_delete.txt"

    # add an additional ext4 partition to the sdcard image in which
    # 'factory_overlay' gets copied into
    cp "$BOARD_DIR/genimage.cfg" factory-genimage.cfg
    patch -p1 < "$BOARD_DIR/factory_overlay_conf.diff"
    create_sdcard  "factory-genimage.cfg" "$BOARD_DIR/factory_install.scr"
}

# Prepare the installer initramfs image
INITRAMFS_DIR=$STAGING_DIR/initramfs/install
(cd $INITRAMFS_DIR && find .) |\
    cpio -oH newc -D $INITRAMFS_DIR |\
    gzip > installer.cpio.gz

create_sdcard  "$BOARD_DIR/genimage.cfg" "$BOARD_DIR/install.scr"
factory_sdcard
