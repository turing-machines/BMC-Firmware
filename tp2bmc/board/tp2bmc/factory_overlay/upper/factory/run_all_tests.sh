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
EXIT_CODE=0

echo "TURING PI FACTORY TESTING STARTED"

try_run_test() {
    local file="$1"
    local retries="$2"

    FILENAME=$(basename "$file")
    local test_id="${FILENAME:0:3}"
    local test_name="${FILENAME:4}"
    test_name="${test_name%%.*}"
    test_name="${test_name//_/ }"

    echo -e "\n* [${test_id}] Running ${test_name}.."
    "$FILE"
    EXIT_CODE=$?

    if [[ $EXIT_CODE -ne 0 ]]; then
        echo -e "  [${test_id}] ${RED}FAILED${NC}"
        if [[ "$retries" -ne 0 ]]; then
            ((retries--))
            confirm "try again?"
            try_run_test "$1" "$retries"
        else
            echo "Exiting program. the tests can be started again with running './run_all_tests.sh'"
            exit 1
        fi
    else
        echo -e "  [${test_id}] ${GREEN}PASSED${NC}"
    fi
}

nr_of_executed_tests=0
# Iterate over files in the test directory that are prefixed with 3 numbers
# followed by a dash. e.g. "999-"
for FILE in "$TEST_DIR"/[0-9][0-9][0-9]-*; do
    # Check if the file exists and is executable
    if [[ -x "$FILE" ]]; then
        try_run_test "$FILE" "2"
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

source install_official_firmware.sh
wait

echo -e "${GREEN}ALL TESTS PASSED (${nr_of_executed_tests})${NC}"
echo "eject the SD card, restart the system"
