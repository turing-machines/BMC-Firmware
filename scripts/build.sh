#!/usr/bin/env bash
# shellcheck shell=bash

set -euo pipefail

# Save current directory
CWD=$(pwd)

# Configure directories
root=$(git rev-parse --show-toplevel)
dist="${root}/dist"
build_root="${root}/buildroot"

# Jump to build directory
cd "${build_root}" || exit 1

# Prepare buildroot
make BR2_EXTERNAL=../tp2bmc tp2bmc_defconfig

# Build
if make; then
    # Check if we are running on darwin (macOS)
    # if we do then use the mounted dist folder, this is the repository directory on the host
    if [[ "${HOST_OS^^}" == "DARWIN" ]]; then
        dist="/mnt/dist"
    fi

    # Create dist folder if not exists
    if [[ ! -d "${dist}" ]]; then
        mkdir -p "${dist}"
    fi

    # Copy to dist folder
    if [[ -d "${build_root}/output/images" ]]; then
        # Check of OTA Image
        if [[ -f "${build_root}/output/images/rootfs.erofs" ]]; then
            # OTA image exists, copy it to dist
            cp -v "${build_root}/output/images/rootfs.erofs" "${dist}/tp2-bmc-ota-$(date +%Y.%m.%d).tpu"
        else
            echo "Error: OTA Image not found"
        fi
    fi
fi

# Restore current directory
cd "${CWD}" || exit 1
