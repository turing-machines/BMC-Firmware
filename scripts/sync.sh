#!/usr/bin/env bash
# shellcheck shell=bash

set -euo pipefail

if [[ "${HOST_OS^^}" == "DARWIN" ]]; then
    echo "Syncing repository to container"
    rsync -hrPti --delete --update --fsync --exclude buildroot /mnt/ /work 

    # Sync container back to repository
    echo "Syncing container back to repository"
    rsync -hrPti --delete --update --fsync --exclude buildroot /work/ /mnt
else
    echo "sync.sh is only used on macOS"
    exit 1
fi
