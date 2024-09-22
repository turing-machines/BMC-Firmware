#!/bin/bash

source helpers.sh

# Define color codes
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
MAGENTA='\033[0;35m'
CYAN='\033[0;36m'
GRAY='\033[0;90m'
NC='\033[0m' # No Color

# Define the directory to search
TEST_DIR="/factory"
NON_EXECUTABLE_FILES=()
EXIT_CODE=0

echo -e "\n\n${RED}TURING PI FACTORY TESTING STARTED${NC}"

#test_results=""

print_running() {
    local test_id="$1"
    local test_name="$2"
    str=$(echo -e "\n* [${test_id}] Running ${test_name}...")
    echo "$str"
    #test_results+="$str"$'\n'
}

print_passed() {
    local test_id="$1"
    local add_to=${2:-test_results}
    str=$(echo -e "  [${test_id}] ${GREEN}PASSED${NC}")
    echo "$str"
    eval "$add_to+=\"\$str\"$'\n'"
}

print_failed() {
    local test_id="$1"
    local context="${2:-}"
    local add_to=${3:-test_results}
    str=$(echo -e "  [${test_id}] ${RED}FAILED${NC}    ${context}")
    echo "$str"
    eval "$add_to+=\"\$str\"$'\n'"
}

print_not_ran() {
    local test_id="$1"
    local add_to=${2:-test_results}
    str=$(echo -e "  [${test_id}] ${GRAY}not ran${NC}")
    echo "$str"
    eval "$add_to+=\"\$str\"$'\n'"
}

try_run_test() {
    local file="$1"
    "$TEST_DIR/$file"
    EXIT_CODE=$?

    return $EXIT_CODE
}

#tpi advanced normal --node 2 >/dev/null
#tpi power on > /dev/null
tpi power --node 1 on >/dev/null &
tpi power --node 2 on >/dev/null &
tpi power --node 3 on >/dev/null &
tpi power --node 4 on >/dev/null &

test_results=""

stage_1=0

print_running "001" "Device Tree and EEPROM test"
try_run_test "001-device_tree_and_eeprom_test.sh"
exit_code=$?
case $exit_code in
    0)
        print_passed "001"
        ;;
    1)
        print_failed "001"
        stage_1=1
        ;;
esac

print_running "002" "RTC test"
try_run_test "002-rtc-test.sh"
exit_code=$?
case $exit_code in
    0)
        print_passed "002"
        ;;
    1)
        print_failed "002"
        stage_1=1
        ;;
esac

if [[ $stage_1 -ne 0 ]]; then
    echo -e "\n${RED}Possibly a hardware issue on the TPi2 board${NC}"
    exit 1
fi

stage_1_test_results="$test_results"
test_results=""

stage_2=0

while true; do
    print_running "003" "BMC Ethernet test"
    try_run_test "003-bmc_ethernet_test.sh"
    exit_code=$?
    case $exit_code in
        0)
            print_passed "003"
            stage_2=0
            ;;
        1)
            print_failed "003" "FIX #01"
            stage_2=1
            ;;
    esac
    
    if [ "$stage_2" -eq 0 ]; then
        break
    fi

    if ! confirm "Try again?"; then
        echo "Please wait, shutting down the nodes safely..."
        bash post_test.sh
        echo "You can disconnect power now"
        exit 1
    fi
done

stage_2_test_results="$test_results"
test_results=""


run_test_005=1
result_test_005=""
run_test_006=1
result_test_006=""
run_test_007=1
result_test_007=""
run_test_008=1
result_test_008=""
run_test_009=1
result_test_009=""
run_test_010=1
result_test_010=""
run_test_011=1
result_test_011=""

while true; do

    stage_3=0

    tpi power --node 1 on >/dev/null &
    tpi power --node 2 on >/dev/null &
    tpi power --node 3 on >/dev/null &
    tpi power --node 4 on >/dev/null &


    if [ "$run_test_005" -eq 1 ]; then
        print_running "005" "Node Ethernet test"
        try_run_test "005-node_ethernet_test.sh"
        exit_code=$?

        node1_not_ok=$((exit_code & 1))
        node2_not_ok=$((exit_code & 2))
        node3_not_ok=$((exit_code & 4))
        node4_not_ok=$((exit_code & 8))

        result_test_005=""
        if [ "$node1_not_ok" -ne 0 ]; then
            print_failed "005-1" "${YELLOW}FIX #11${NC}" result_test_005
            stage_3=1
        fi
        if [ "$node2_not_ok" -ne 0 ]; then
            print_failed "005-2" "${BLUE}FIX #12${NC}" result_test_005
            stage_3=1
        fi
        if [ "$node3_not_ok" -ne 0 ]; then
            print_failed "005-3" "${MAGENTA}FIX #13${NC}" result_test_005
            stage_3=1
        fi
        if [ "$node4_not_ok" -ne 0 ]; then
            print_failed "005-4" "${GREEN}FIX #14${NC}" result_test_005
            stage_3=1
        fi
        if [ "$node1_not_ok" -eq 0 ] && [ "$node2_not_ok" -eq 0 ] && [ "$node3_not_ok" -eq 0 ] && [ "$node4_not_ok" -eq 0 ]; then
            print_passed "005" result_test_005
            run_test_005=0
        fi
    fi

    #print_running "###" "peripheral tests"

    if [ "$run_test_006" -eq 1 ] && [ "$node1_not_ok" -eq 0 ]; then
        #print_running "006" "Node1 Peripheral test"
        out1=$(mktemp -u)
        err1=$(mktemp -u)
        try_run_test "006-node1_peripheral_test.sh" >"$out1" 2>"$err1" &
        pid1=$!
    fi
    if [ "$run_test_007" -eq 1 ] && [ "$node2_not_ok" -eq 0 ]; then
        #print_running "007" "Node2 Peripheral test"
        out2=$(mktemp -u)
        err2=$(mktemp -u)
        try_run_test "007-node2_peripheral_test.sh" >"$out2" 2>"$err2" &
        pid2=$!
    fi
    if [ "$run_test_008" -eq 1 ] && [ "$node3_not_ok" -eq 0 ]; then
        #print_running "008" "Node3 Peripheral test"
        out3=$(mktemp -u)
        err3=$(mktemp -u)
        try_run_test "008-node3_peripheral_test.sh" >"$out3" 2>"$err3" &
        pid3=$!
    fi
    if [ "$run_test_009" -eq 1 ] && [ "$node4_not_ok" -eq 0 ]; then
        #print_running "009" "Node1 Peripheral test"
        out4=$(mktemp -u)
        err4=$(mktemp -u)
        try_run_test "009-node4_peripheral_test.sh" >"$out4" 2>"$err4" &
        pid4=$!
    fi

    if [ "$run_test_006" -eq 1 ]; then
        print_running "006" "Node1 Peripheral test"
        result_test_006=""
        if [ "$node1_not_ok" -eq 0 ]; then
            wait $pid1
            status1=$?
            cat "$out1"
            cat "$err1"
            case $status1 in
                0)
                    print_passed "006" result_test_006
                    run_test_006=0
                    ;;
                1)
                    print_failed "006" "${YELLOW}FIX #11 #21 #31${NC}" result_test_006
                    stage_3=1
                    ;;
            esac
        else
            print_not_ran "006" result_test_006
        fi
    fi
    if [ "$run_test_007" -eq 1 ]; then
        print_running "007" "Node2 Peripheral test"
        result_test_007=""
        if [ "$node2_not_ok" -eq 0 ]; then
            wait $pid2
            status2=$?
            cat "$out2"
            cat "$err2"
            case $status2 in
                0)
                    print_passed "007" result_test_007
                    run_test_007=0
                    ;;
                1)
                    print_failed "007" "${BLUE}FIX #12 #22 #32${NC}" result_test_007
                    stage_3=1
                    ;;
            esac
            #tpi advanced msd --node 2 >/dev/null &
            #pid2=$!
        else
            print_not_ran "007" result_test_007
        fi
    fi

    advanced=0
    if [ "$run_test_011" -eq 1 ]; then
        tpi advanced msd --node 2 >/dev/null &
        pid2=$!
        advanced=1
    fi

    if [ "$run_test_008" -eq 1 ]; then
        print_running "008" "Node3 Peripheral test"
        result_test_008=""
        if [ "$node3_not_ok" -eq 0 ]; then
            wait $pid3
            status3=$?
            cat "$out3"
            cat "$err3"
            case $status3 in
                0)
                    print_passed "008" result_test_008
                    run_test_008=0
                    ;;
                1)
                    print_failed "008" "${MAGENTA}FIX #13 #23${NC}" result_test_008
                    stage_3=1
                    ;;
                2)
                    print_failed "008" "${MAGENTA}FIX #13 #33${NC}" result_test_008
                    stage_3=1
                    ;;
                3)
                    print_failed "008" "${MAGENTA}FIX #13 #23 #33${NC}" result_test_008
                    stage_3=1
                    ;;
            esac
        else
            print_not_ran "008" result_test_008
        fi
    fi
    if [ "$run_test_009" -eq 1 ]; then
        print_running "009" "Node4 Peripheral test"
        result_test_009=""
        if [ "$node4_not_ok" -eq 0 ]; then
            wait $pid4
            status4=$?
            cat "$out4"
            cat "$err4"
            case $status4 in
                0)
                    print_passed "009" result_test_009
                    run_test_009=0
                    ;;
                1)
                    print_failed "009" "${GREEN}FIX #14 #24${NC}" result_test_009
                    stage_3=1
                    ;;
                2)
                    print_failed "009" "${GREEN}FIX #14 #34${NC}" result_test_009
                    stage_3=1
                    ;;
                3)
                    print_failed "009" "${GREEN}FIX #14 #24 #34${NC}" result_test_009
                    stage_3=1
                    ;;
            esac
        else
            print_not_ran "009" result_test_009
        fi
    fi

    if [ "$run_test_010" -eq 1 ]; then
        print_running "010" "Node1 USB Host test"
        result_test_010=""
        if [ "$node1_not_ok" -eq 0 ]; then
            try_run_test "010-node1_usb_host_test.sh"
            exit_code=$?
            case $exit_code in
                0)
                    print_passed "010" result_test_010
                    run_test_010=0
                    ;;
                1)
                    print_failed "010" "FIX #02" result_test_010
                    stage_3=1
                    ;;
            esac
        else
            print_not_ran "010" result_test_010
        fi
    fi

    if [ "$run_test_011" -eq 1 ]; then
        print_running "011" "USB_DEV test"
        result_test_011=""
        if [ "$node1_not_ok" -eq 0 ]; then
            wait $pid2
            tpi usb device --node 2 > /dev/null
            sleep 0.5
            try_run_test "010-usb_dev_test.sh"
            exit_code=$?
            case $exit_code in
                0)
                    print_passed "011" result_test_011
                    run_test_011=0
                    ;;
                1)
                    print_failed "011" "FIX #02" result_test_011
                    stage_3=1
                    ;;
            esac
        else
            print_not_ran "011" result_test_011
        fi
    fi

    if [ "$run_test_005" -eq 1 ] || [ "$run_test_007" -eq 1 ]; then
        if [ "$advanced" -eq 1 ]; then
            wait $pid2
        fi
        tpi advanced normal --node 2 >/dev/null
        pid2=$!
    fi

    stage_3_test_results="$result_test_005$result_test_006$result_test_007$result_test_008$result_test_009$result_test_010$result_test_011"
    final_result="$stage_1_test_results$stage_2_test_results$stage_3_test_results"

    echo -e "\n\n\n---------------------------------------------"
    echo -e "\n${YELLOW}All test results:${NC}"
    echo -e "$final_result"

    if [ "$stage_3" -eq 0 ]; then
        break
    fi

    if ! confirm "Try again?"; then
        echo "Please wait, shutting down the nodes safely..."
        bash post_test.sh
        echo "You can disconnect power now"
        exit 1
    fi
    
    
    if [ "$node1_not_ok" -eq 1 ] || [ "$node2_not_ok" -eq 1 ] || [ "$node3_not_ok" -eq 1 ] || [ "$node4_not_ok" -eq 1 ]; then
        run_test_005=1
    fi

    if [ "$run_test_005" -eq 1 ] || [ "$run_test_007" -eq 1 ]; then
        wait $pid2
    fi
    
done

source install_official_firmware.sh
wait

echo -e "${GREEN}ALL TESTS PASSED${NC}"
echo -e "${GREEN}Eject the SD card, restart the system${NC}\n\n"
