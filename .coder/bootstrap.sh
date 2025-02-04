#!/usr/bin/env bash
# shellcheck shell=bash

CLONE_PATH="${CLONE_PATH:-$(cd "$(dirname "${BASH_SOURCE[0]}")/../" && pwd)}"

echo "Configuring Git"
git -C "${CLONE_PATH}" config include.path ../.gitconfig

echo "Configure Build Environment"
sudo apt-get update
sudo apt-get -qq install -y \
    build-essential \
    subversion \
    git-core \
    libncurses5-dev \
    zlib1g-dev \
    gawk \
    bc \
    cpio \
    file \
    flex \
    quilt \
    libssl-dev \
    xsltproc \
    libxml-parser-perl \
    mercurial \
    bzr \
    ecj \
    cvs \
    unzip \
    zlib1g-dev \
    libncurses-dev \
    u-boot-tools \
    mkbootimg \
    xxd \
    shellcheck

case "$(uname)" in
    Linux)
        OS=Linux
    ;;
    Darwin)
        OS=Darwin
    ;;
esac

case "$(uname -m)" in
    aarch64|arm64)
        ARCH=arm64
    ;;
    x86_64)
        ARCH=x86_64
    ;;
esac

echo "Installing Hadolint" 
sudo wget -q -O /usr/bin/hadolint "https://github.com/hadolint/hadolint/releases/download/v2.12.0/hadolint-${OS}-${ARCH}"
sudo chmod 755 /usr/bin/hadolint

echo "Coder Environment Initialized"
