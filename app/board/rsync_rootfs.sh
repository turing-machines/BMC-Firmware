#!/bin/sh

#project="/home/wille/development/sdk/T113_PRO/buildroot-100ask_t113-pro/"
project="/home/wenyi/development/github/turing-pi/"
# echo ${project}app/board/rootfs_overlay/
# echo ${project}/br2t113pro/board/100ask/rootfs_overlay/
rsync -av ${project}app/board/rootfs_overlay/  ${project}br2t113pro/board/100ask/rootfs_overlay/
