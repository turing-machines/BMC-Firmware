#!/usr/bin/env bash
# shellcheck shell=bash

set -euo pipefail

# Save current directory
CWD=$(pwd)

# Jump to build directory
cd "$(git rev-parse --show-toplevel)" || exit 1

# Prepare buildroot
make BR2_EXTERNAL=../tp2bmc tp2bmc_defconfig

# Build
make

# Restore current directory
cd "$CWD" || exit 1
