#!/usr/bin/env bash
# shellcheck shell=bash

set -meuo pipefail

root="$(cd "$(dirname "${BASH_SOURCE[0]}")/../" && pwd)"

echo "Enforcing permissions"
sudo chmod 777 /work /mnt
sudo chown vscode:vscode /work

# Configure git config
# initialize git config before initial sync of .git directory
git config --local include.path ../.gitconfig

if [[ "${HOST_OS^^}" == "DARWIN" ]]; then
    # Sync Git only for initial sync
    if [[ ! -d "/work/.git" ]]; then
        rsync -hrPti --delete --update --fsync --exclude buildroot /mnt/ /work > /tmp/sync.log
    fi
    
    # Sync container and host
    "${root}/scripts/sync.sh"
fi

echo "Container initialized"
