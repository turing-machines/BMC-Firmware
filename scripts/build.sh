#!/usr/bin/env bash
# shellcheck shell=bash

set -eo pipefail

# Save current directory
CWD=$(pwd)

# Configure directories
root="$(cd "$(dirname "${BASH_SOURCE[0]}")/../" && pwd)"
dist="${root}/dist"
build_root="${root}/buildroot"
release="$(date +'%Y.%m.%d'-"$(git rev-parse --short=8 HEAD)")"

# Function to display usage
usage() {
    echo "Usage: $0 [--dir|-d <directory>] [--release|-r] [--help|-h]"
    exit 1
}

# Parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --dir|-d)
            if [[ -n "$2" ]]; then
                build_root="$2"
                shift 2
            else
                shift 1
            fi
            ;;
        --release|-r)
            if [[ -n "$2" ]]; then
                release="$2"
                shift 2
            else
                shift 1
            fi
            ;;
        --help|-h)
            usage
            ;;
        *)
            usage
            ;;
    esac
done

# Jump to build directory
cd "${build_root}" || exit 1

# Prepare buildroot
make BR2_EXTERNAL=../tp2bmc tp2bmc_defconfig

# Build
if make; then
    OTA_FILENAME="tp2-bmc-firmware-ota-${release}.tpu"
    SDCARD_FILENAME="tp2-bmc-firmware-sdcard-${release}.img"

    # Check if we are running on darwin (macOS)
    # if we do then use the mounted dist folder, this is the repository directory on the host
    if [[ "${HOST_OS^^}" == "DARWIN" ]]; then
        dist="/mnt/dist"
    fi

    # Create dist folder if not exists
    if [[ ! -d "${dist}" ]]; then
        mkdir -p "${dist}"
    fi

    printf '\n\n'
    printf '================================================================================\n'
    printf 'Build Completed\n\n'

    # Copy to dist folder
    if [[ -d "${build_root}/output/images" ]]; then
        # Check for OTA Image
        if [[ -f "${build_root}/output/images/rootfs.erofs" ]]; then
            # OTA image exists, copy it to dist
            echo "Copying OTA image"
            cp -v "${build_root}/output/images/rootfs.erofs" "${dist}/${OTA_FILENAME}"
            
            # The image is a binary image therefor use sha256sum binary mode
            echo "Generating SHA256 for: ${OTA_FILENAME}.sha256"
            sha256sum -b "${dist}/${OTA_FILENAME}" > "${dist}/${OTA_FILENAME}.sha256"
        else
            echo "Error: OTA image not found"
        fi

        # Check for SDCard image
        if [[ -f "${build_root}/output/images/tp2-bmc-firmware-sdcard.img" ]]; then
            # SDCard image, copy it to dist
            echo "Copying SDCard image"
            cp -v "${build_root}/output/images/tp2-bmc-firmware-sdcard.img" "${dist}/${SDCARD_FILENAME}"

            # The image is a binary image therefor use sha256sum binary mode
            echo "Generating SHA256 for: ${SDCARD_FILENAME}"
            sha256sum -b "${dist}/${SDCARD_FILENAME}" > "${dist}/${SDCARD_FILENAME}.sha256"
        else
            echo "Error: SDCard image not found"
        fi
    fi

    # If on macOS sync to Host
    if [[ "${HOST_OS^^}" == "DARWIN" ]]; then
        "${root}/scripts/sync.sh"
    fi

    # Summary
    printf '\n\n'
    printf '================================================================================\n'
    printf 'Summary\n\n'

    printf "%s\n\n" "$(du -h "${dist}/${OTA_FILENAME}" "${dist}/${SDCARD_FILENAME}")"
fi

# Restore current directory
cd "${CWD}" || exit 1
