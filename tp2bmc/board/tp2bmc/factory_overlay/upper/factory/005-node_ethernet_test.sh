#!/bin/bash

source helpers.sh
source config.sh


retrieve_ip_from_uart() {
    local n="$1"
    local tty="/dev/ttyS${n}"

    USERNAME="$RK1_USERNAME"
    PASSWORD="$RK1_PASSWORD"

    # login
    sleep 2
    echo "${USERNAME}" > "${tty}"
    sleep 1
    echo "${PASSWORD}" > "${tty}"
    sleep 3
    print_ipv4="ip -4 addr show  | grep -oP '(?<=inet\\s)\\d+(\\.\\d+){3}'"
    echo "$print_ipv4" > "/dev/ttyS${n}"
    sleep 1
    return_line=$(tpi uart --node "${n}" get | tail -n 2 | head -n 1)
    echo "$return_line" | grep -oE '([0-9]{1,3}\.){3}[0-9]{1,3}'
}

get_ip_and_ping() {
    local n="$1"
    local ip

    wait_until_booted $n
    if [[ $? -ne 0 ]]; then
        echo "Error: Timeout occurred waiting for node ${n} to boot"
        exit 1
    fi

    local node_ip_var="NODE${n}_IP"
    local node_ip_value=$(eval echo \$$node_ip_var)

    if [ -z "$node_ip_value" ]; then
        echo -e "\tnode ${n}: getting ip from uart"
        ip=$(retrieve_ip_from_uart "$n")
    else
        ip="$node_ip_value"
    fi

    # Ping the IP address
    ping -c 4 -i 0.1 "$ip" > /dev/null
    if [ "$?" -ne 0 ]; then
        echo "Error: could not ping node $n"
        return 1
    fi

    echo -e "\tNode $n $ip => OK"
}

node1_test() {
    get_ip_and_ping "1"
}

node2_test() {
    get_ip_and_ping "2"
}

node3_test() {
    get_ip_and_ping "3"
}

node4_test() {
    get_ip_and_ping "4"
}

#tpi power off > /dev/null
#tpi power on > /dev/null

echo -e "\tWaiting for the modules..."

node1_test & pid1=$!
node2_test & pid2=$!
node3_test & pid3=$!
node4_test & pid4=$!

result=0

wait $pid1
status1=$?
if [ $status1 -ne 0 ]; then
    result=$((result + 1))
fi

wait $pid2
status2=$?
if [ $status2 -ne 0 ]; then
    result=$((result + 2))
fi

wait $pid3
status3=$?
if [ $status3 -ne 0 ]; then
    result=$((result + 4))
fi

wait $pid4
status4=$?
if [ $status4 -ne 0 ]; then
    result=$((result + 8))
fi

exit $result
