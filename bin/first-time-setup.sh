#!/usr/bin/sh

WALLPAPER_FILE="/usr/share/backgrounds/default.png"

trap 'killall -9 kwin_wayland calamares plasmashell mutter labwc || true' EXIT SIGINT SIGTERM

# Wait for the drivers to load
udevadm settle

for i in $(seq 1 50); do
    if [ -e /dev/dri/by-path/platform-*gpu-card ] &&
        [ -e /dev/dri/by-path/platform-*display-subsystem-card ]; then
        break
    fi
    sleep 0.1
done
# If we time out, just go ahead anyways and hope software rendering works.

# Configure the default keyboard layout
country="00"
for country_file in $(find /sys/devices/platform -name country -path '*05AC:*'); do
    [ -z "$country_file" ] && continue
    cc=$(cat "$country_file")
    if [ "$cc" != "00" ]; then
        country="$cc"
    fi
done

case "$country" in
0d)
    xkbmodel=applealu_iso
    ;;
0f)
    xkbmodel=applealu_jis
    ;;
21)
    xkbmodel=applealu_ansi
    ;;
esac

kblang=unk
if [ -e /proc/device-tree/chosen/asahi,kblang-code ]; then
    kblang="$(xxd -ps /proc/device-tree/chosen/asahi,kblang-code)"
fi

case "$kblang" in
00000001) xkblayout=de ;;
00000002) xkblayout=fr ;;
00000003) xkblayout=jp ;;
00000004)
    xkblayout=us
    xkbvariant=intl
    ;;
00000005) xkblayout=us ;;
00000006) xkblayout=gb ;;
00000007) xkblayout=es ;;
00000008) xkblayout=se ;;
00000009) xkblayout=it ;;
0000000a)
    xkblayout=ca
    xkbvariant=multi
    ;;
0000000b) xkblayout=cn ;;
0000000c) xkblayout=dk ;;
0000000d) xkblayout=be ;;
0000000e) xkblayout=no ;;
0000000f) xkblayout=kr106 ;;
00000010) xkblayout=nl ;;
00000011) xkblayout=ch ;;
00000012) xkblayout=tw ;;
00000013) xkblayout=ara ;;
00000014) xkblayout=bg ;;
00000015) xkblayout=hr ;;
00000016) xkblayout=cz ;;
00000017) xkblayout=gr ;;
00000018) xkblayout=il ;;
00000019) xkblayout=is ;;
0000001a) xkblayout=hu ;;
0000001b) xkblayout=pl ;;
0000001c) xkblayout=pt ;;
0000001d) xkblayout=ir ;;
0000001e) xkblayout=ro ;;
0000001f)
    xkblayout=ru
    xkbvariant=mac
    ;;
00000020) xkblayout=sk ;;
00000021) xkblayout=th ;;
00000022) xkblayout=tr ;; # "Turkish-QWERTY-PC"?
00000023) xkblayout=tr ;; # "Turkish"?
00000024)
    xkblayout=ua
    xkbvariant=macOS
    ;;
00000025) xkblayout=tr ;; # "Turkish-Standard"?
00000026) xkblayout=latam ;;
esac

if [ -n "$xkblayout" ] && [ -n "$xkbmodel" ]; then
    localectl set-x11-keymap $xkblayout $xkbmodel $xkbvariant
fi

# Create a dummy home directory for Calamares
export HOME="/run/user/0/calamares-home"
rm -rf "$HOME"
mkdir -p "$HOME"
cd $HOME
mkdir -p .config

# Set up dummy XDG runtime directory so we don't mess with the real root one
export XDG_RUNTIME_DIR="$HOME/.runtime"
mkdir -p "$XDG_RUNTIME_DIR"
chmod 0700 "$XDG_RUNTIME_DIR"

# Detect HiDPI embedded screens and configure things appropriately
HIDPI=0
if [ -e /sys/class/drm/card*-eDP-1/modes ]; then
    WIDTH=$(sort -n /sys/class/drm/card*-eDP-1/modes | sort -n | tail -n 1 | cut -dx -f1)
    echo "Screen width: $WIDTH"
    if [ "$WIDTH" -gt 2048 ]; then
        HIDPI=1
    fi
fi

cat >.config/kdeglobals <<EOF
[KDE]
AnimationDurationFactor=0.5
EOF

cat >.config/breezerc <<EOF
[Windeco Exception 0]
BorderSize=0
Enabled=true
ExceptionPattern=io.calamares.calamares
ExceptionType=0
HideTitleBar=true
Mask=0
EOF

# Configure Plasma
cat >.config/plasma-org.kde.plasma.desktop-appletsrc <<EOF
[ActionPlugins][0]
MiddleButton;NoModifier=
RightButton;NoModifier=
wheel:Vertical;NoModifier=

[Containments][1]
activityId=1d1809e0-5c93-4423-b0f3-93646408cfa6
formfactor=0
immutability=1
lastScreen=0
location=0
plugin=org.kde.desktopcontainment
wallpaperplugin=org.kde.image

[Containments][1][Wallpaper][org.kde.image][General]
Image=file://$WALLPAPER_FILE

[ScreenMapping]
itemsOnDisabledScreens=
EOF

cat >.config/kactivitymanagerdrc <<EOF
[activities]
1d1809e0-5c93-4423-b0f3-93646408cfa6=Default

[main]
currentActivity=1d1809e0-5c93-4423-b0f3-93646408cfa6
EOF

# Set up user session environment, so stuff launches properly
export WAYLAND_DISPLAY=wayland-0
export QT_QPA_PLATFORM=wayland
systemctl --user import-environment WAYLAND_DISPLAY QT_QPA_PLATFORM XDG_RUNTIME_DIR HOME

# Start KWin
if [ -x /usr/bin/mutter ]; then
    mutter --wayland &
elif [ -x /usr/bin/hyprland ]; then
    # reset WAYLAND_DISPLAY for labwc to start up
    unset WAYLAND_DISPLAY
    labwc &
else
    kwin_wayland --drm --no-global-shortcuts --no-lockscreen --locale1 &
fi

# Wait for the compositor to be available
for i in $(seq 1 50); do
    [ -e "$XDG_RUNTIME_DIR/$WAYLAND_DISPLAY" ] && break
    sleep 0.1
done

if [ ! -e "$XDG_RUNTIME_DIR/$WAYLAND_DISPLAY" ]; then
    echo "windowmanager failed to start!"
    exit 1
fi

# Start plasmashell to get a wallpaper
if [ ! -x /usr/bin/mutter ]; then
    # Configure display scale if needed
    if [ "$HIDPI" == 1 ]; then
        kscreen-doctor output.eDP-1.scale.1.5
    fi

    plasmashell &
fi

# Delay a bit for visual consistency
sleep 0.3

# Background the app and wait on it. This makes killing this script
# clean up properly.
calamares -c /usr/share/calamares-asahi &

# Disable the setup wizard if it completes successfully
if wait $!; then
    # Work around https://bugs.kde.org/show_bug.cgi?id=475435
    xkbmodel="$(localectl status | grep "X11 Model" | cut -d: -f2 | sed 's/^ //')"
    userhome="$(ls -d /home/* | head -1)"
    if [ -n "$xkbmodel" ] && [ -e "$userhome" ]; then
        uidgid="$(stat -c %u:%g "$userhome")"
        mkdir -p "$userhome/.config"
        cat >"$userhome/.config/kxkbrc" <<EOF
[Layout]
Model=$xkbmodel
EOF
        chown "$uidgid" "$userhome/.config" "$userhome/.config/kxkbrc"
    fi
    # we installed mutter temporarily for calamares on cosmic, so remove it after we're finished with calamares
    if [ -x /usr/bin/cosmic-comp ]; then
        pacman -Rc --noconfirm mutter || true
        # also remove any other unneeded dependencies that got installed with mutter
        pacman -Qdtq | xargs -ro pacman -Rns --noconfirm || true
    fi
    systemctl disable calamares.service
    usermod -p '*' root
fi
