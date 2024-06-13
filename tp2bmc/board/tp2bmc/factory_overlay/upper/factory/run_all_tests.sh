#!/bin/bash

source helpers.sh

# Define color codes
RED='\033[0;31m'
YELLOW='\033[0;33m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

# Define the directory to search
TEST_DIR="/factory"
NON_EXECUTABLE_FILES=()
FAILED_TESTS=()
EXIT_CODE=0

echo "TURING PI FACTORY TESTING STARTED"

nr_of_executed_tests=0
# Iterate over files in the test directory that are prefixed with 3 numbers
# followed by a dash. e.g. "999-"
for FILE in "$TEST_DIR"/[0-9][0-9][0-9]-*; do
    # Check if the file exists and is executable
    if [[ -x "$FILE" ]]; then
		FILENAME=$(basename "$FILE")
		test_id="${FILENAME:0:3}"
		test_name="${FILENAME:4}"
		test_name="${test_name%%.*}"
        test_name="${test_name//_/ }"

		echo -e "\n* [${test_id}] Running ${test_name}.."
        "$FILE"
        EXIT_CODE=$?


		echo -ne "  [${test_id}] "
        if [[ $EXIT_CODE -ne 0 ]]; then
            echo -e "${RED}FAILED${NC} exited with code $EXIT_CODE"
            FAILED_TESTS+=("$FILE")
		else
            echo -e "${GREEN}PASSED${NC}"
        fi
        # Increment the counter
        ((nr_of_executed_tests++))
    else
        NON_EXECUTABLE_FILES+=("$FILE")
    fi
done

if [[ ${#NON_EXECUTABLE_FILES[@]} -ne 0 ]]; then
    echo -e "${YELLOW}Warning: skipped the following files as they are not executable or do not exist:${NC}"
    for NON_EXECUTABLE_FILE in "${NON_EXECUTABLE_FILES[@]}"; do
        echo -e "\t$NON_EXECUTABLE_FILE"
    done
fi

echo "running post test script.."
./post_test.sh &

if [[ ${#FAILED_TESTS[@]} -eq 0 ]]; then
    echo -ne "${GREEN}"
else
    echo -ne "${RED}"
fi

failed_count=$(printf '%s\n' "${FAILED_TESTS[@]}" | wc -w)
passed=$((nr_of_executed_tests - failed_count))
echo -e "FINISHED TESTS:\t(${passed}/${nr_of_executed_tests}) tests passed"

if [[ ${#FAILED_TESTS[@]} -ne 0 ]]; then
    echo "Failed tests:"
    for FAILED_TEST in "${FAILED_TESTS[@]}"; do
        FILENAME=$(basename "$FAILED_TEST")
        test_name="${FILENAME:4}"
        test_name="${test_name%%.*}"
        test_name="${test_name//_/ }"
        echo -e "\t$test_name"
    done
    echo -en "${NC}"

    confirm "You have failed tests, Do you still want to flash firmware to the board?"
    if [[ $? -ne 0 ]]; then
        exit 1
    fi
else
    echo "You are about to install the official firmware.."
    confirm
    if [[ $? -ne 0 ]]; then
        exit 1
    fi
fi

source install_official_firmware.sh
echo "THIS IS THE END, press the power button on the front panel to test power down"
wait
