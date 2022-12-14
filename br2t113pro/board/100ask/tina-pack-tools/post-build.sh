#!/bin/sh
BOARD_DIR="$(dirname $0)"

# For debug
echo "Target binary dir $BOARD_DIR"

# Copy Files to BINARY
cp $BOARD_DIR/bin/* -rfvd  $BINARIES_DIR

#Copy boot files.
cp $BOARD_DIR/../t113-pro/* -rfvd  $BINARIES_DIR

#cd buildroot/output/images/
cd $BINARIES_DIR

#build env.fex bootargs.
mkenvimage -r -p 0x00 -s 0x20000 -o env.fex env.cfg

#build uboot optee files.
$BINARIES_DIR/dragonsecboot  -pack boot_package.cfg

#buildroot kernel boot images.
mkbootimg --kernel zImage  --ramdisk  ramdisk.img --board sun8iw20p1 --base  0x40200000 --kernel_offset  0x0 --ramdisk_offset  0x01000000 -o  boot.img
