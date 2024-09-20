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
            return 1
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

    retries=0
    while true; do
        output=$(sshpass -p "$RK1_PASSWORD" ssh -o LogLevel=ERROR -o UserKnownHostsFile=/dev/null -o StrictHostKeyChecking=no -o ConnectTimeout=1 "${RK1_USERNAME}@${node_ip_value}" "$cmd" 2>/dev/null)
        return_code=$?
        if [ $return_code -eq 0 ] || [ $return_code -eq 1 ]; then
            echo "$output"
            return $return_code
        fi
        (( retries += 1 ))
        if [ $retries -eq 500 ]; then
            break
        fi
        sleep 0.1
    done
    return $return_code
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
    #local INTERVAL=5
    #local END_TIME=$((SECONDS + PING_TIMEOUT))

    local counter=$PING_TIMEOUT
    while ! ping -c 1 -W 1 $HOST &> /dev/null; do
        if [ $counter -eq 0 ]; then
            echo "Error: node $node ($HOST) is not reachable within $PING_TIMEOUT seconds.">&2
            return 1
        fi
        #sleep $INTERVAL
        ((counter--))
    done
}
