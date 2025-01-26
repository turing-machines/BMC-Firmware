#!/usr/bin/env bash
# shellcheck shell=bash

set -euo pipefail

# Save current directory
CWD=$(pwd)

# Configure directories
ROOT=$(git rev-parse --show-toplevel)
DIST="${ROOT}/dist"
BUILD_ROOT="${ROOT}/buildroot"

# Jump to build directory
cd "${BUILD_ROOT}" || exit 1

# Prepare buildroot
make BR2_EXTERNAL=../tp2bmc tp2bmc_defconfig

# Build
if make; then
    # Check if we are running on darwin (macOS)
    # if we do then use the mounted dist folder, this is the repository directory on the host
    if [[ "${HOST_OS^^}" == "DARWIN" ]]; then
        DIST="/mnt/dist"
    fi

    # Create dist folder if not exists
    if [[ ! -d "${DIST}" ]]; then
        mkdir -p "$DIST"
    fi

    # Copy to dist folder
    if [[ -d "${BUILD_ROOT}/output/images" ]]; then
        # Check of OTA Image
        if [[ -f "${BUILD_ROOT}/output/images/rootfs.erofs" ]]; then
            # OTA image exists, copy it to dist
            cp -v "${BUILD_ROOT}/output/images/rootfs.erofs" "${DIST}/tp2-bmc-ota-$(date +%Y.%m.%d).tpu"
        else
            echo "Error: OTA Image not found"
        fi
    fi
fi

# Restore current directory
cd "$CWD" || exit 1
