#!/bin/sh

systemctl disable calamares.service && usermod -p '*' root
systemctl -i reboot
