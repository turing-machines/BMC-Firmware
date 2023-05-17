#!/bin/sh
BOARD_DIR="$(dirname $0)/../../../br2t113pro/board/100ask/t113-pro"


# For debug
echo "Target binary dir $BOARD_DIR"

cp $BOARD_DIR/env-spinand.cfg -rfvd  $BINARIES_DIR
cp $BOARD_DIR/boot_package.cfg -rfvd  $BINARIES_DIR
cp $BOARD_DIR/bootlogo.bmp.lzma -rfvd  $BINARIES_DIR
cp $BOARD_DIR/bootlogo.bmp -rfvd  $BINARIES_DIR
cp $BOARD_DIR/ramdisk.img -rfvd  $BINARIES_DIR

# Copy some system bins.
cp $BOARD_DIR/../pack_img/* -rfvd  $BINARIES_DIR

#Copy tina pack tools
cp $BOARD_DIR/../tina-pack-tools/* -rfvd  $BINARIES_DIR


#cd buildroot/output/images/
cd $BINARIES_DIR

# Patch boot_package.cfg to refer to the new dtb filename.
# This should be removed once the br2t113pro directory is deleted.
sed -i s/sun8iw20p1-t113-100ask-t113-pro/sun8iw20p1-t113-turingmachines-tp2bmc/ boot_package.cfg

#build env.fex bootargs.
mkenvimage -r -p 0x00 -s 0x20000 -o env.fex env-spinand.cfg

#build uboot optee files.
$BINARIES_DIR/dragonsecboot  -pack boot_package.cfg

#buildroot kernel boot images.
mkbootimg --kernel zImage  --ramdisk  ramdisk.img --board sun8iw20p1 --base  0x40200000 --kernel_offset  0x0 --ramdisk_offset  0x01000000 -o  boot.img
