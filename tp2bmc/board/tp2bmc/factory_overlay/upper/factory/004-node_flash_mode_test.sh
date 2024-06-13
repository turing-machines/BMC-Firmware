#!/bin/bash
source helpers.sh

get_latest_device_sys() {
    ls -lt /sys/class/block | grep -v 'total' | head -n 1
}

verify_msd_connection() {
    partition="$1"
    file1="$2"
    file2="$3"
    file3="$4"

    mount_point=$(mktemp -d)
    if [ ! -e "/dev/$partition" ]; then
        echo "Error: Could not find mount /dev/$partition"
        exit 1
    fi

    mount "/dev/$partition" "$mount_point"
    res=$?
    if [ "$res" -ne 0 ]; then
        echo "error mounting $partition"
        exit $res
    fi 

    if [ ! -f "$mount_point/$file1" ] && \
        [ ! -f "$mount_point/$file2" ] && \
        [ ! -f "$mount_point/$file3" ]; then
            echo "$(ls -l $mount_point)"
            echo "Error: cannot find $file1 or $file2 on mount $partition"
            exit 1
    fi

    umount "$mount_point"
    rmdir "$mount_point"
}

tpi power off > /dev/null
last_added_device=$(get_latest_device_sys)

for n in {1..4}
do
    res=$(tpi advanced msd --node "${n}")
    exit_code=$?
    sleep 3
    new_block=$(get_latest_device_sys)
    if [[ "$last_added_device" == "$new_block" ]]; then
        echo "$res"
        echo "Error: can not detect node $n over USB"
        tpi power off > /dev/null
        exit 1
    fi

    partition=$(echo "$new_block" | awk '{print $9}')
    # start.elf should be present on the boot partition of the cm4 rpi
    # network-config should be present on the RK1
    verify_msd_connection "${partition}1" "rk3588-turing-rk1.dtb" "network-config" "bcm2711-rpi-cm4.dtb"

    tpi power off --node "${n}" > /dev/null
    tpi usb device --node "${n}" > /dev/null
    echo -e "\tNode $n => OK"
done

