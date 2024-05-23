#!/bin/bash

set -e

# Check if /sys/class/rtc/rtc0 exists
if [ -d "/sys/class/rtc/rtc0" ]; then
    echo -e "\tRTC Information:"
    
    # Read and print various RTC attributes
    echo -e "\tName: $(cat /sys/class/rtc/rtc0/name)"
    #echo "Date and Time: $(cat /sys/class/rtc/rtc0/date) $(cat /sys/class/rtc/rtc0/time)"
    #echo "Alarm: $(cat /sys/class/rtc/rtc0/wakealarm)"
    #echo "Since Epoch: $(cat /sys/class/rtc/rtc0/since_epoch)"
    echo -e "\tMax User Freq: $(cat /sys/class/rtc/rtc0/max_user_freq)"
else
    echo "Error: /sys/class/rtc/rtc0 directory does not exist. No RTC information available."
    exit 1
fi

name=$(cat /sys/class/rtc/rtc0/name | awk '{print $1}')

if [[ "$name" != "rtc-pcf8563" ]]; then
    echo "Error: rtc clock `rtc-pcf8563` could not be found"
    exit 1
fi
