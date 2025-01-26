#!/usr/bin/env bash
# shellcheck shell=bash

set -euo pipefail

ROOT=$(git rev-parse --show-toplevel)

if [[ -d "${ROOT}/buildroot" ]]; then
    rm -rf "${ROOT}/buildroot"
fi
