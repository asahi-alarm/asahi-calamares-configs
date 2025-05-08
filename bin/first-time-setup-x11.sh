#!/bin/sh

mkdir -p feh
cat >feh/buttons <<EOF
pan
zoom
menu
EOF

XFCE=/usr/share/backgrounds/xfce/xfce-blue.jpg
LXQT=/usr/share/lxqt/wallpapers/waves-logo.png
MATE=/usr/share/backgrounds/mate/desktop/GreenTraditional.jpg

for candidate in "$XFCE" "$MATE" "$LXQT"; do
  if [ -f "$candidate" ]; then
    feh --zoom fill -N -F "$candidate" &
  fi
done

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
        DEFAULT_XKBMODEL=applealu_iso
        ;;
    0f)
        DEFAULT_XKBMODEL=applealu_jis
        ;;
    21)
        DEFAULT_XKBMODEL=applealu_ansi
        ;;
esac

KBLANG_CODE=unk
if [ -e /proc/device-tree/chosen/asahi,kblang-code ]; then
    KBLANG_CODE="$(xxd -ps /proc/device-tree/chosen/asahi,kblang-code)" 
fi

case "$KBLANG_CODE" in
    00000001) DEFAULT_XKBLAYOUT=de;;
    00000002) DEFAULT_XKBLAYOUT=fr;;
    00000003) DEFAULT_XKBLAYOUT=jp;;
    00000004) DEFAULT_XKBLAYOUT=us; DEFAULT_XKBVARIANT=intl;;
    00000005) DEFAULT_XKBLAYOUT=us;;
    00000006) DEFAULT_XKBLAYOUT=gb;;
    00000007) DEFAULT_XKBLAYOUT=es;;
    00000008) DEFAULT_XKBLAYOUT=se;;
    00000009) DEFAULT_XKBLAYOUT=it;;
    0000000a) DEFAULT_XKBLAYOUT=ca; DEFAULT_XKBVARIANT=multi;;
    0000000b) DEFAULT_XKBLAYOUT=cn;;
    0000000c) DEFAULT_XKBLAYOUT=dk;;
    0000000d) DEFAULT_XKBLAYOUT=be;;
    0000000e) DEFAULT_XKBLAYOUT=no;;
    0000000f) DEFAULT_XKBLAYOUT=kr106;;
    00000010) DEFAULT_XKBLAYOUT=nl;;
    00000011) DEFAULT_XKBLAYOUT=ch;;
    00000012) DEFAULT_XKBLAYOUT=tw;;
    00000013) DEFAULT_XKBLAYOUT=ara;;
    00000014) DEFAULT_XKBLAYOUT=bg;;
    00000015) DEFAULT_XKBLAYOUT=hr;;
    00000016) DEFAULT_XKBLAYOUT=cz;;
    00000017) DEFAULT_XKBLAYOUT=gr;;
    00000018) DEFAULT_XKBLAYOUT=il;;
    00000019) DEFAULT_XKBLAYOUT=is;;
    0000001a) DEFAULT_XKBLAYOUT=hu;;
    0000001b) DEFAULT_XKBLAYOUT=pl;;
    0000001c) DEFAULT_XKBLAYOUT=pt;;
    0000001d) DEFAULT_XKBLAYOUT=ir;;
    0000001e) DEFAULT_XKBLAYOUT=ro;;
    0000001f) DEFAULT_XKBLAYOUT=ru DEFAULT_XKBVARIANT=mac;;
    00000020) DEFAULT_XKBLAYOUT=sk;;
    00000021) DEFAULT_XKBLAYOUT=th;;
    00000022) DEFAULT_XKBLAYOUT=tr;; # "Turkish-QWERTY-PC"?
    00000023) DEFAULT_XKBLAYOUT=tr;; # "Turkish"?
    00000024) DEFAULT_XKBLAYOUT=ua DEFAULT_XKBVARIANT=macOS;;
    00000025) DEFAULT_XKBLAYOUT=tr;; # "Turkish-Standard"?
    00000026) DEFAULT_XKBLAYOUT=latam;;
esac

export DEFAULT_XKBMODEL DEFAULT_XKBLAYOUT DEFAULT_XKBVARIANT

calamares -c /usr/share/calamares-asahi/ &

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
    systemctl disable calamares-x11.service
    usermod -p '*' root
fi
