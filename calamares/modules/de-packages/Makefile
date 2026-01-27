# SPDX-FileCopyrightText: no
# SPDX-License-Identifier: CC0-1.0

TARGET = libcalamares_viewmodule_depackages.so

SOURCES = DePackagesViewStep.cpp
HEADERS = DePackagesViewStep.h
OBJECTS = $(SOURCES:.cpp=.o)
MOC_SOURCES = moc_DePackagesViewStep.cpp
MOC_OBJECTS = $(MOC_SOURCES:.cpp=.o)

QT_CFLAGS := $(shell pkg-config --cflags Qt6Core Qt6Widgets)
QT_LIBS := $(shell pkg-config --libs Qt6Core Qt6Widgets)

CALAMARES_INCLUDE = /usr/include/libcalamares

CXX = g++
MOC = /usr/lib/qt6/moc

CXXFLAGS = -std=c++17 -fPIC -Wall -Wextra -O2 \
           $(QT_CFLAGS) \
           -I$(CALAMARES_INCLUDE) \
           -DPLUGGINDLLEXPORT_PRO \
           -DQT_PLUGIN

LDFLAGS = -shared $(QT_LIBS) -L/usr/lib -lcalamares -lcalamaresui

INSTALL_DIR = /usr/lib/calamares/modules/de-packages

.PHONY: all clean install

all: $(TARGET)

$(TARGET): $(OBJECTS) $(MOC_OBJECTS)
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

moc_%.cpp: %.h
	$(MOC) $(QT_CFLAGS) -I$(CALAMARES_INCLUDE) $< -o $@

# Dependencies
DePackagesViewStep.o: DePackagesViewStep.cpp DePackagesViewStep.h
moc_DePackagesViewStep.o: moc_DePackagesViewStep.cpp

clean:
	rm -f $(OBJECTS) $(MOC_OBJECTS) $(MOC_SOURCES) $(TARGET)

install: $(TARGET)
	install -d $(DESTDIR)$(INSTALL_DIR)
	install -m755 $(TARGET) $(DESTDIR)$(INSTALL_DIR)/$(TARGET)
	install -m644 module.desc $(DESTDIR)$(INSTALL_DIR)/module.desc
