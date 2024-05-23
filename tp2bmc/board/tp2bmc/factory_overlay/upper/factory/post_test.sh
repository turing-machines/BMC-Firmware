#!/bin/bash
source helpers.sh
shutdown="sudo shutdown 0"

for n in {1..4}
do
    echo "$RK1_USERNAME" > "/dev/ttyS${n}"
    sleep 1
    echo "$RK1_PASSWORD" > "/dev/ttyS${n}"
    sleep 2
done

for n in {1..4}
do
    echo "$shutdown" > "/dev/ttyS${n}"
    echo "$RK1_PASSWORD" > "/dev/ttyS${n}"
    sleep 1
done

sleep 7
