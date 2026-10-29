#!/usr/bin/sh
# SPDX-License-Identifier: MIT
#
# Robust package installation with retry logic for unreliable repos

set -e

LOG=/var/log/asahi-install-packages.log

# Re-run ourselves with all output mirrored into a log file opened with
# O_SYNC, so it survives a crash of cage/calamares or the user rebooting
# mid-install. tee -p keeps feeding the log when calamares (our stdout) dies.
if [ -z "$ASAHI_INSTALL_LOGGING" ]; then
    status_file=$(mktemp)
    {
        echo "=== $(date -Is) $0 ==="
        rc=0
        ASAHI_INSTALL_LOGGING=1 "$0" "$@" || rc=$?
        echo "$rc" >"$status_file"
        echo "=== $(date -Is) exit status $rc ==="
    } 2>&1 | {
        tee -p /dev/fd/3 | dd of="$LOG" oflag=append,sync conv=notrunc status=none
    } 3>&1
    rc=$(cat "$status_file" 2>/dev/null || echo 1)
    rm -f "$status_file"
    exit "$rc"
fi

MAX_RETRIES=5
RETRY_DELAY=10

# Read the package list written by the de-packages module
PACKAGES=$(cat /tmp/calamares-packages 2>/dev/null || echo "")

if [ -z "$PACKAGES" ]; then
    echo "Error: No packages to install (missing /tmp/calamares-packages)"
    exit 1
fi

DE=$(cat /tmp/calamares-de 2>/dev/null || echo "unknown")
DM=$(cat /tmp/calamares-dm 2>/dev/null || echo "")
echo "Installing packages for: $DE"
echo "Packages: $PACKAGES"

# Expand groups into their members.
#
# pacman refuses the whole transaction when a single group member has
# unresolvable dependencies: it asks ":: Do you want to skip the above
# packages? [y/N]" and --noconfirm answers with the default, N. Expanding
# groups ourselves lets us drop only the broken members below.
expand_groups() {
    for item in "$@"; do
        members=$(pacman -Sqg "$item" 2>/dev/null) || members=""
        if [ -n "$members" ]; then
            echo "$members"
        else
            echo "$item"
        fi
    done
}

# Drop packages whose dependencies cannot be satisfied by the current repos,
# so a stale repo entry (e.g. gnome-books -> webkit2gtk -> dropped libsoup)
# does not abort the install of everything else.
filter_resolvable() {
    for pkg in "$@"; do
        if pacman -Sp --noconfirm "$pkg" >/dev/null 2>&1; then
            echo "$pkg"
        else
            echo "Skipping $pkg: dependencies cannot be satisfied" >&2
        fi
    done
}

# Retry loop for package installation
attempt=1
INSTALL_LIST=$PACKAGES
while [ $attempt -le $MAX_RETRIES ]; do
    echo ""
    echo "=== Installation attempt $attempt of $MAX_RETRIES ==="
    echo ""

    # Sync database first (double -y to force refresh)
    echo "Synchronizing package database..."
    if ! pacman -Syy --noconfirm --disable-download-timeout; then
        echo "Warning: Database sync failed, retrying..."
        sleep $RETRY_DELAY
        attempt=$((attempt + 1))
        continue
    fi

    # Packages published after the image was built may be signed by keys the
    # image does not know yet, so update the installed keyrings first
    keyrings=$(pacman -Qq | grep -- '-keyring$')
    if [ -n "$keyrings" ]; then
        echo "Updating keyrings: $keyrings"
        pacman -S --noconfirm --needed --disable-download-timeout $keyrings ||
            echo "Warning: keyring update failed, continuing"
    fi

    # Upgrade the system together with the new packages: installing from
    # freshly synced repos on top of the image's older packages would be a
    # partial upgrade
    echo "Installing packages..."
    if pacman -Su --noconfirm --needed --disable-download-timeout $INSTALL_LIST; then
        echo ""
        echo "=== Package installation successful ==="
        exit 0
    fi

    echo ""
    echo "Installation attempt $attempt failed."

    # The failure may be a broken repo entry rather than a flaky mirror, in
    # which case plain retries can never succeed. Rebuild the list without the
    # packages the freshly synced databases cannot resolve.
    echo "Rebuilding package list without unresolvable packages..."
    INSTALL_LIST=$(filter_resolvable $(expand_groups $PACKAGES))

    if [ -z "$INSTALL_LIST" ]; then
        echo "Error: none of the requested packages can be installed"
        exit 1
    fi

    # A desktop without its display manager is not a usable install, so treat
    # that as fatal rather than silently skipping it like any other package.
    if [ -n "$DM" ] && ! echo "$INSTALL_LIST" | grep -qx "$DM"; then
        echo "Error: display manager $DM cannot be installed"
        exit 1
    fi

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
echo "A full log of this run is kept in $LOG."
exit 1
