#!/bin/bash

source config.sh

try_again() {
    if [ -n "$1" ]; then
        echo "=> $1"
    fi

    while true; do
        read -p "Do you want to try again? [Y/n]: " -r REPLY
        # If the user presses Enter or types 'y'/'Y'
        if [[ -z "$REPLY" || "$REPLY" =~ ^[Yy]$ ]]; then
            return 0
        # If the user types 'n'/'N'
        elif [[ "$REPLY" =~ ^[Nn]$ ]]; then
            return 1
        else
            # Prompt the user to try again for invalid input
            echo "Invalid input, please try again."
        fi
    done
}

confirm() {
    question="Do you want to continue?"
    if [ -n "$1" ]; then
        question="$1"
    fi

    while true; do
        read -p "=> $question [Y/n]: " -r REPLY
        # If the user presses Enter or types 'y'/'Y'
        if [[ -z "$REPLY" || "$REPLY" =~ ^[Yy]$ ]]; then
            return 0
        # If the user types 'n'/'N'
        elif [[ "$REPLY" =~ ^[Nn]$ ]]; then
            exit 1
        else
            # Prompt the user to try again for invalid input
            echo "Invalid input, please try again."
        fi
    done
}

send_command() {
    local n="$1"
    local cmd="$2"

    if [ -z "$RK1_USERNAME" ];then
        echo "set \$RK1_USERNAME in config.sh"
        return 1
    fi

    if [ -z "$RK1_PASSWORD" ];then
        echo "set \$RK1_PASSWORD in config.sh"
        return 1
    fi

    local node_ip_var="NODE${n}_IP"
    local node_ip_value=$(eval echo \$$node_ip_var)

    sshpass -p "$RK1_PASSWORD" ssh -o StrictHostKeyChecking=no "${RK1_USERNAME}@${node_ip_value}" "$cmd"
}

uart_output_node() {
    local n="$1"
    tpi uart get --node ${n}  | head 
}

wait_until_booted() {
    local node="$1"
    local node_ip_var="NODE${n}_IP"
    local node_ip_value=$(eval echo \$$node_ip_var)

    local HOST="$node_ip_value"
    local INTERVAL=5
    local END_TIME=$((SECONDS + PING_TIMEOUT))

    while ! ping -c 1 -W 1 $HOST &> /dev/null; do
        if [ $SECONDS -ge $END_TIME ]; then
            echo "Error: node $node ($HOST) is not reachable within $PING_TIMEOUT seconds."
            exit 1
        fi
        sleep $INTERVAL
    done
}

get_usb_devices() {
    local node="$1"

    result=$(send_command $node "lsusb")
    if [[ $? -ne 0 ]]; then
        echo "Error: USB3X2 NODE4 ports error: ${result}"
        exit 1
    fi
    # dont include the controller on the bus
    echo "$result" | grep -v 'Device 001'
}

get_pci_devices() {
    local n="$1"
    result=$(send_command "$n" "lspci | grep -v RK3588 | grep -v 'PCI bridge: Broadcom Inc'")
    if [[ $? -ne 0 ]]; then
        echo "Error: could not get pci information on Node ${n}"
        exit 1
    fi
    echo "$result"
}
