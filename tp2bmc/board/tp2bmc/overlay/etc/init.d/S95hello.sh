#!/bin/sh

start() {
    mkdir -p /mnt/sdcard

    if [ -b /dev/mmcblk0p1 ]; then
        mount /dev/mmcblk0p1 /mnt/sdcard
    elif [ -b /dev/mmcblk0 ]; then
        mount /dev/mmcblk0 /mnt/sdcard
    fi

    echo " _____ _   _ ____  ___ _   _  ____ "
    echo "|_   _| | | |  _ \|_ _| \ | |/ ___|"
    echo "  | | | | | | |_) || ||  \| | |  _ "
    echo "  | | | |_| |  _ < | || |\  | |_| |"
    echo "  |_|  \___/|_| \_\___|_| \_|\____|"

    echo 3 4 1 7 > /proc/sys/kernel/printk

    sleep 1
    /etc/setStaticNet.sh

    bmc &

    /etc/test_ping.sh &
}

if [ "$1" = "start" ]; then
    start
fi
