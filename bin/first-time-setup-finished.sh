#!/bin/sh

systemctl disable calamares.service calamares-x11.service
pacman -Rdd calamares
usermod -p '*' root
systemctl -i reboot
