# SPDX-FileCopyrightText: no
# SPDX-License-Identifier: CC0-1.0

TARGET = libcalamares_viewmodule_networksetup.so

SOURCES = NetworkSetupViewStep.cpp NetworkSetupPage.cpp
HEADERS = NetworkSetupViewStep.h NetworkSetupPage.h
OBJECTS = $(SOURCES:.cpp=.o)
MOC_SOURCES = moc_NetworkSetupViewStep.cpp moc_NetworkSetupPage.cpp
MOC_OBJECTS = $(MOC_SOURCES:.cpp=.o)

# Qt6
QT_CFLAGS := $(shell pkg-config --cflags Qt6Core Qt6Widgets Qt6DBus)
QT_LIBS := $(shell pkg-config --libs Qt6Core Qt6Widgets Qt6DBus)

# Calamares
CALAMARES_INCLUDE = /usr/include/libcalamares

CXX = g++
MOC = /usr/lib/qt6/moc

CXXFLAGS = -std=c++17 -fPIC -Wall -Wextra -O2 \
           $(QT_CFLAGS) \
           -I$(CALAMARES_INCLUDE) \
           -DPLUGGINDLLEXPORT_PRO \
           -DQT_PLUGIN

LDFLAGS = -shared $(QT_LIBS) -L/usr/lib -lcalamares -lcalamaresui

INSTALL_DIR = /usr/lib/calamares/modules/networksetup

.PHONY: all clean install

all: $(TARGET)

$(TARGET): $(OBJECTS) $(MOC_OBJECTS)
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

moc_%.cpp: %.h
	$(MOC) $(QT_CFLAGS) -I$(CALAMARES_INCLUDE) $< -o $@

# Dependencies
NetworkSetupViewStep.o: NetworkSetupViewStep.cpp NetworkSetupViewStep.h NetworkSetupPage.h
NetworkSetupPage.o: NetworkSetupPage.cpp NetworkSetupPage.h
moc_NetworkSetupViewStep.o: moc_NetworkSetupViewStep.cpp
moc_NetworkSetupPage.o: moc_NetworkSetupPage.cpp

clean:
	rm -f $(OBJECTS) $(MOC_OBJECTS) $(MOC_SOURCES) $(TARGET)

install: $(TARGET)
	install -d $(DESTDIR)$(INSTALL_DIR)
	install -m755 $(TARGET) $(DESTDIR)$(INSTALL_DIR)/$(TARGET)
	install -m644 module.desc $(DESTDIR)$(INSTALL_DIR)/module.desc
