#!/bin/bash

set -e
source helpers.sh
confirm "Do you see video on the HDMI monitor?"

tpi power on > /dev/null
confirm "Press 'Key1'. Do the red light go off?"
