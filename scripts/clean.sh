#!/usr/bin/env bash
# shellcheck shell=bash

set -euo pipefail

# Get repository root
root=$(git rev-parse --show-toplevel)
# root directory fallback
if [[ -z "${root}" ]]; then
    root="$(cd "$(dirname "${BASH_SOURCE[0]}")/../" && pwd)"
    echo "Using root directory: ${root}"
fi

if [[ -d "${root}/buildroot" ]]; then
    rm -rf "${root}/buildroot"
fi

if [[ -d "${root}/dist" ]]; then
    rm -rf "${root}/dist"
fi
