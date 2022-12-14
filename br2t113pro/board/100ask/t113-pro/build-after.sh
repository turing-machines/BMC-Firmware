#!/bin/sh
BOARD_DIR="$(dirname $0)"

# For debug
echo "Target binary dir $BOARD_DIR"

# Copy Files to BINARY
cp $BOARD_DIR/../dragon/* -rfvd  $BINARIES_DIR

cd  $BINARIES_DIR
rm boot.fex
rm rootfs-ubifs.fex
wait
ln -s boot.img boot.fex
ln -s rootfs.ubifs rootfs-ubifs.fex

echo "-----------------update_mbr--------------------------"
$BINARIES_DIR/update_mbr  sys_partition.bin 4

echo "-----------------dragon pack--------------------------"
$BINARIES_DIR/dragon  image.cfg sys_partition.fex

