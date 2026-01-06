#!/bin/sh

# we installed mutter temporarily for calamares on cosmic, so remove it after we're finished with calamares
if [ -x /usr/bin/cosmic-comp ]; then
  pacman -Rc --noconfirm mutter || true
  # also remove any other unneeded dependencies that got installed with mutter
  pacman -Qdtq | xargs -ro pacman -Rns --noconfirm || true
fi
systemctl disable calamares.service calamares-x11.service
usermod -p '*' root
systemctl -i reboot
