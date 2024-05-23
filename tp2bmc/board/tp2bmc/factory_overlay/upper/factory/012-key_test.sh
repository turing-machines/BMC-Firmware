#!/bin/bash

set -e
source helpers.sh
tpi power on > /dev/null
confirm "Press 'Key1'. Do the red light go off?"
