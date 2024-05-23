#!/bin/bash
source config.sh

# Extract the gateway address for the eth0 interface
gateway=$(ip route show dev eth0 | grep default | awk '{print $3}')
ip=$(ip -o -4 addr show eth0 | awk '{print $4}' | cut -d/ -f1)

if [ "$ip" == "$NODE1_IP" ] || [ "$ip" == "$NODE2_IP" ] || \
    [ "$ip" == "$NODE3_IP" ] || [ "$ip" == "$NODE4_IP" ]; then
    echo "Error: Turing Pi has same IP ($ip) as one of the modules!!"
    exit 1
fi

# Check if the gateway was found
if [ -z "$gateway" ]; then
    echo "No gateway found for eth0"
    exit 1
fi

mac=$(ifconfig eth0 | grep HWaddr | awk '{print $5}')
echo -e "\tinterface:\t'eth0'"
echo -e "\tMAC: \t\t'${mac}'"

ping -c 4 "${gateway}" > /dev/null

