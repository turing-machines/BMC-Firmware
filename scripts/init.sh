#!/usr/bin/env bash
# shellcheck shell=bash

set -meuo pipefail

echo "Enforcing permissions"
sudo chmod 777 /work /mnt
sudo chown vscode:vscode /work

if [[ "${HOST_OS^^}" == "DARWIN" ]]; then
    # Sync repository to container
    echo "Syncing repository to container"
    rsync -hvrPt --update --exclude buildroot /mnt/ /work 

    # Sync container back to repository
    echo "Syncing container back to repository"
    rsync -hvrPt --update --exclude buildroot /work/ /mnt
fi

echo "Container initialized"
