#!/usr/bin/sh

trap 'killall -9 cage calamares foot || true' EXIT SIGINT SIGTERM

# Wait for the drivers to load
udevadm settle

for i in $(seq 1 50); do
    if [ -e /dev/dri/by-path/platform-*gpu-card ]; then
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

# Set up dummy XDG runtime directory so we don't mess with the real root one
export XDG_RUNTIME_DIR="$HOME/.runtime"
mkdir -p "$XDG_RUNTIME_DIR"
chmod 0700 "$XDG_RUNTIME_DIR"

# Detect HiDPI embedded screens and configure scaling
if [ -e /sys/class/drm/card*-eDP-1/modes ]; then
    WIDTH=$(sort -n /sys/class/drm/card*-eDP-1/modes | sort -n | tail -n 1 | cut -dx -f1)
    echo "Screen width: $WIDTH"
    if [ "$WIDTH" -gt 2048 ]; then
        export WLR_OUTPUT_SCALE=1.5
        export QT_SCALE_FACTOR=1.5
    fi
fi

# Ensure cage runs directly on DRM, not nested
unset DISPLAY
unset WAYLAND_DISPLAY

# Set Qt to use Wayland (cage will set WAYLAND_DISPLAY for the child process)
export QT_QPA_PLATFORM=wayland
# Force Fusion style to avoid theme issues with dark/light colors
export QT_STYLE_OVERRIDE=Fusion
export QT_QPA_PLATFORMTHEME=

# Launch cage with Calamares
# cage runs directly on DRM/KMS and sets up Wayland for its child
# -s disables output scaling
cage -s -- calamares -D8 -c /usr/share/calamares-asahi
