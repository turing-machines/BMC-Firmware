#!/bin/bash

set -euo pipefail
cd "${0%/*}"

FDT_FILENAME=$(basename $TARGET_DIR/boot/*.dtb)
sed s/FDT_FILENAME_HERE/${FDT_FILENAME}/g < boot.scr > /tmp/boot.scr
mkimage -A arm -T script -d /tmp/boot.scr $TARGET_DIR/boot/boot.scr.uimg
