#!/usr/bin/env bash
# shellcheck shell=bash

set -euo pipefail

echo "Syncing repository to container"
rsync -hrPti --delete --update --fsync --exclude buildroot /mnt/ /work 

# Sync container back to repository
echo "Syncing container back to repository"
rsync -hrPti --delete --update --fsync --exclude buildroot /work/ /mnt
