#!/usr/bin/env bash
# shellcheck shell=bash

# This script downloads, unpacks and installs the buildroot system to a given
# location. The default install location will be used when no install dir is
# passed, this is the root directory of this repository.
#
# Usage:
#
#   ./setup_build.sh <install dir> (optional)
#

# Configure shell
set -eo pipefail
set -x

# Buildroot Version
BUILDROOT_VER="2024.05.1"

# Save current directory
CWD=$(pwd)

download_dir=$(mktemp -d)
install_dir="$1"
buildroot_url="https://buildroot.org/downloads/buildroot-${BUILDROOT_VER}.tar.gz"
buildroot=$(basename "$buildroot_url")
buildroot_folder="${buildroot%.tar.gz}"
project_root=$(git rev-parse --show-toplevel)

# Fix for GitHub Actions: root directory fallback
if [[ -z "${project_root}" ]]; then
    project_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/../" && pwd)"
    echo "Using root directory: ${project_root}"
fi

pushd "$download_dir"
    wget "$buildroot_url"
    tar -xvf "$buildroot"
popd

if [ -z "$install_dir" ]; then
    # try to install it into the base-dir of the git repo
    install_dir="$project_root"
fi

if [ -e "$install_dir/buildroot" ]; then
    rm -rf "$install_dir/buildroot"
fi

mv "$download_dir/$buildroot_folder" "$install_dir/buildroot"

pushd "$install_dir/buildroot"
for patchfile in "$project_root"/buildroot_patches/*; do
    if [ -f "$patchfile" ]; then
        patch -p1 < "$patchfile"
    fi
done
popd

# Restore current directory
cd "${CWD}" || exit 1
