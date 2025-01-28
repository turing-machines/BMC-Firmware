#!/usr/bin/env bash
# shellcheck shell=bash

set -meuo pipefail

root=$(git rev-parse --show-toplevel)
# root directory fallback
if [[ -z "${root}" ]]; then
    root="$(cd "$(dirname "${BASH_SOURCE[0]}")/../" && pwd)"
    echo "Using root directory: ${root}"
fi

echo "Enforcing permissions"
sudo chmod 777 /work /mnt
sudo chown vscode:vscode /work

if [[ "${HOST_OS^^}" == "DARWIN" ]]; then
    # Sync Git only for initial sync
    if [[ ! -d "/work/.git" ]]; then
        rsync -hrPti --delete --update --fsync --exclude buildroot /mnt/ /work > /tmp/sync.log
    fi
    
    # Sync container and host
    "{root}/scripts/sync.sh"
fi

# Configure git config
git config --local include.path ../.gitconfig

echo "Container initialized"
