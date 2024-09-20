#!/bin/bash
# This test makes use of the AT command shell to detect if there is SIM card
# inserted. According the documentation of Quectel, there are 2 tty's created,
# where the second one is the AT shell.
# tested with:  Quectel EC25 LTE modem
exit 0
source node_helpers.sh
at_modem="/dev/ttyUSB2"

# make sure the AT shell is not in use
send_command "1" "sudo killall ModemManager" > /dev/null 2>&1

status=$(send_command "1" "echo -e 'AT+CPIN?\r' > ${at_modem} && timeout 2s cat ${at_modem}")

# 10 = SIM NOT INSERTED
if [[ "$status" == *"ERROR: 10"* ]] ||\
         [[ "$status" == *"NOT INSERTED"* ]] ||\
         [[ "$status" == *"ERROR"* ]] ||\
         [[ -z "$status" ]]; then
    echo -e "Error: Cannot detect SIM ($status)"
    exit 1
fi
