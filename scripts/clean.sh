#!/usr/bin/env bash
# shellcheck shell=bash

set -eo pipefail

# Get repository root
root="$(cd "$(dirname "${BASH_SOURCE[0]}")/../" && pwd)"

if [[ -d "${root}/buildroot" ]]; then
    rm -rf "${root}/buildroot"
fi

if [[ -d "${root}/dist" ]]; then
    rm -rf "${root}/dist"
fi
