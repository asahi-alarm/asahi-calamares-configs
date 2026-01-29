#!/usr/bin/sh
# SPDX-License-Identifier: MIT
#
# Robust package installation with retry logic for unreliable repos

set -e

MAX_RETRIES=5
RETRY_DELAY=10

# Read the package list written by the de-packages module
PACKAGES=$(cat /tmp/calamares-packages 2>/dev/null || echo "")

if [ -z "$PACKAGES" ]; then
    echo "Error: No packages to install (missing /tmp/calamares-packages)"
    exit 1
fi

DE=$(cat /tmp/calamares-de 2>/dev/null || echo "unknown")
echo "Installing packages for: $DE"
echo "Packages: $PACKAGES"

# Retry loop for package installation
attempt=1
while [ $attempt -le $MAX_RETRIES ]; do
    echo ""
    echo "=== Installation attempt $attempt of $MAX_RETRIES ==="
    echo ""

    # Sync database first (double -y to force refresh)
    echo "Synchronizing package database..."
    if ! pacman -Syy --noconfirm; then
        echo "Warning: Database sync failed, retrying..."
        sleep $RETRY_DELAY
        attempt=$((attempt + 1))
        continue
    fi

    # Try to install packages
    echo "Installing packages..."
    if pacman -S --noconfirm --needed $PACKAGES; then
        echo ""
        echo "=== Package installation successful ==="
        exit 0
    fi

    echo ""
    echo "Installation attempt $attempt failed."

    if [ $attempt -lt $MAX_RETRIES ]; then
        echo "Waiting ${RETRY_DELAY}s before retry..."
        sleep $RETRY_DELAY
    fi

    attempt=$((attempt + 1))
done

echo ""
echo "=== All $MAX_RETRIES installation attempts failed ==="
echo ""
echo "Please check your internet connection and mirror configuration."
echo "You can try running 'pacman -Syyu' manually after rebooting."
exit 1
