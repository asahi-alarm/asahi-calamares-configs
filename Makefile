PREFIX=/usr

SCRIPTS=bin/first-time-setup-cage.sh bin/asahi-de-configure.sh bin/asahi-install-packages.sh
UNITS=calamares-cage.service
MULTI_USER_WANTS=calamares-cage.service

.PHONY: all build install uninstall clean

all: build

build:
	$(MAKE) -C calamares/modules/networksetup
	$(MAKE) -C calamares/modules/de-packages

install: build
	install -d $(DESTDIR)$(PREFIX)/bin/
	install -m0755 -t $(DESTDIR)$(PREFIX)/bin/ $(SCRIPTS)
	install -dD $(DESTDIR)$(PREFIX)/lib/systemd/system
	install -m0644 -t $(DESTDIR)$(PREFIX)/lib/systemd/system $(addprefix systemd/,$(UNITS))
	install -d $(DESTDIR)$(PREFIX)/share/calamares-asahi/
	cp -r calamares/* $(DESTDIR)$(PREFIX)/share/calamares-asahi/
	# Install custom de-packages module to calamares modules directory
	install -d $(DESTDIR)$(PREFIX)/lib/calamares/modules/de-packages/
	install -m0644 calamares/modules/de-packages/module.desc $(DESTDIR)$(PREFIX)/lib/calamares/modules/de-packages/
	install -m0755 calamares/modules/de-packages/libcalamares_viewmodule_depackages.so $(DESTDIR)$(PREFIX)/lib/calamares/modules/de-packages/
	# Install custom networksetup module
	install -d $(DESTDIR)$(PREFIX)/lib/calamares/modules/networksetup/
	install -m0644 calamares/modules/networksetup/module.desc $(DESTDIR)$(PREFIX)/lib/calamares/modules/networksetup/
	install -m0755 calamares/modules/networksetup/libcalamares_viewmodule_networksetup.so $(DESTDIR)$(PREFIX)/lib/calamares/modules/networksetup/

uninstall:
	rm -f $(addprefix $(DESTDIR)$(PREFIX)/bin/,$(SCRIPTS))
	rm -f $(addprefix $(DESTDIR)$(PREFIX)/lib/systemd/system/,$(UNITS))
	rm -f $(addprefix $(DESTDIR)$(PREFIX)/lib/systemd/system/multi-user.target.wants/,$(MULTI_USER_WANTS))
	rm -rf $(DESTDIR)$(PREFIX)/share/calamares/{branding/asahi,settings.conf,modules}

clean:
	$(MAKE) -C calamares/modules/networksetup clean
	$(MAKE) -C calamares/modules/de-packages clean
