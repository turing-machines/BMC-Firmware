#!/bin/bash

set -euo pipefail
# buildroots uboot-tools are ancient. Use the one from our uboot build.
mkimage="$BUILD_DIR/uboot-*/tools/mkimage"

cd "${0%/*}"
mkdir -p $TARGET_DIR/boot/
$mkimage -A arm -T script -d boot.scr $TARGET_DIR/boot/boot.scr.uimg

cp $PWD/*.its "$BINARIES_DIR/"
cd "$BINARIES_DIR"
$mkimage -E -f "turing-pi2.its" "$TARGET_DIR/boot/turing-pi2.itb"

if [ -e ${TARGET_DIR}/etc/inittab ]; then
	grep -qE '^GS0::' ${TARGET_DIR}/etc/inittab || \
    sed -i '/GENERIC_SERIAL/a\
GS0::respawn:/sbin/getty -L ttyGS0 115200 vt100 # BMC-USB-OTG' ${TARGET_DIR}/etc/inittab
fi


# tries to retrieve the official firmware version and print it into
# /etc/os-release
overwrite_os_release() {
    if [[ -n "$BUILD_VERSION" ]]; then
        build_version="$BUILD_VERSION"
    else
        build_version=$(git describe --tags)
        if [[ $? -ne 0 ]]; then
            echo "Error: Failed to get version from git describe"
            build_vrsion="v2-unknown"
        fi
    fi

    os_release_file="/etc/os-release"

    echo "NAME=turingpi" > "$os_release_file"
    echo "PRETTY_NAME=Turing Pi ${build_version%%-*}" >> "$os_release_file"
    echo "VERSION=${build_version}" >> "$os_release_file"
    echo "Firmware version set to ${build_version}"
}

overwrite_os_release
