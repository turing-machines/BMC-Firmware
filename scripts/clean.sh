#!/usr/bin/env bash
# shellcheck shell=bash

set -euo pipefail

# Get repository root
root=$(git rev-parse --show-toplevel)

if [[ -d "${root}/buildroot" ]]; then
    rm -rf "${root}/buildroot"
fi
