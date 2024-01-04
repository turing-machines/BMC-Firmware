#!/bin/bash

download_dir=$(mktemp -d)
install_dir="$1"
buildroot_url='https://buildroot.org/downloads/buildroot-2023.08.tar.gz'
buildroot=$(basename "$buildroot_url")
buildroot_folder="${buildroot%.tar.gz}"
project_root="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

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
cd "$project_root" && patch -p1 < "$project_root"/buildroot_patches/*
