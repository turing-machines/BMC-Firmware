#!/usr/bin/env bash
# shellcheck shell=bash

set -meuo pipefail

echo "Enforcing permissions"
sudo chmod 777 /work /mnt
sudo chown vscode:vscode /work

if [[ "${HOST_OS^^}" == "DARWIN" ]]; then
    # Sync Git only for initial sync
    if [[ ! -d "/work/.git" ]]; then
        rsync -hrPti --delete --update --fsync --exclude buildroot /mnt/ /work > /tmp/sync.log
    fi
    
    # Sync container and host
    "$(git rev-parse --show-toplevel)/scripts/sync.sh"
fi

# Configure git config
git config --local include.path ../.gitconfig

echo "Container initialized"
