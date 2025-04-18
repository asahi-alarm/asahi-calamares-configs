#!/bin/sh

systemctl disable calamares.service calamares-x11.service
usermod -p '*' root
systemctl -i reboot
