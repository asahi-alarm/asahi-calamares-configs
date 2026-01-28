# SPDX-FileCopyrightText: no
# SPDX-License-Identifier: CC0-1.0

calamares_add_plugin(networksetup
    TYPE viewmodule
    EXPORT_MACRO PLUGINDLLEXPORT_PRO
    SOURCES
        NetworkSetupViewStep.cpp
        NetworkSetupPage.cpp
    LINK_PRIVATE_LIBRARIES
        Qt::DBus
    SHARED_LIB
)
