#!/usr/bin/env bash
# shellcheck shell=bash

set -euo pipefail

# Save current directory
CWD=$(pwd)

# Jump to build directory
BUILD_ROOT="$(git rev-parse --show-toplevel)/buildroot"
cd "${BUILD_ROOT}" || exit 1

# Prepare buildroot
make BR2_EXTERNAL=../tp2bmc tp2bmc_defconfig

# Build
if make; then
    DIST=$(git rev-parse --show-toplevel)/dist
    if [[ "${HOST_OS^^}" == "DARWIN" ]]; then
        DIST="/mnt/dist"
    fi
    if [[ ! -d "${DIST}" ]]; then
        mkdir -p "$DIST"
    fi

    # Copy to dist folder
    if [[ -d "${BUILD_ROOT}/output/images" ]]; then
        # Check of OTA Image
        if [[ -f "${BUILD_ROOT}/output/images/rootfs.erofs" ]]; then
            cp -v "${BUILD_ROOT}/output/images/rootfs.erofs" "${DIST}/tp2-bmc-ota-$(date +%Y.%m.%d).tpu"
        else
            echo "Error: OTA Image not found"
        fi
    fi
fi

# Restore current directory
cd "$CWD" || exit 1
