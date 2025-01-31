#!/usr/bin/env bash
# shellcheck shell=bash

set -eo pipefail

if [[ "${HOST_OS^^}" == "DARWIN" ]]; then
    # Sync container back to repository
    echo "Syncing container back to repository"
    rsync -hrPti --delete --update --fsync --exclude buildroot --exclude .git /work/ /mnt > /tmp/sync.log
    
    echo "Syncing repository to container"
    rsync -hrPti --delete --update --fsync --exclude buildroot --exclude .git /mnt/ /work > /tmp/sync.log
else
    echo "sync.sh is only used on macOS"
    exit 1
fi
