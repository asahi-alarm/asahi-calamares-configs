PREFIX=/usr/local

SCRIPTS=bin/first-time-setup.sh bin/first-time-setup-x11.sh bin/first-time-setup-finished.sh
UNITS=calamares.service calamares-x11.service
MULTI_USER_WANTS=calamares.service

install:
	install -d $(DESTDIR)$(PREFIX)/bin/
	install -m0755 -t $(DESTDIR)$(PREFIX)/bin/ $(SCRIPTS)
	install -dD $(DESTDIR)$(PREFIX)/lib/systemd/system
	install -m0644 -t $(DESTDIR)$(PREFIX)/lib/systemd/system $(addprefix systemd/,$(UNITS))
	install -d $(DESTDIR)$(PREFIX)/share/calamares-asahi/
	cp -r calamares/* $(DESTDIR)$(PREFIX)/share/calamares-asahi/

uninstall:
	rm -f $(addprefix $(DESTDIR)$(PREFIX)/bin/,$(SCRIPTS))
	rm -f $(addprefix $(DESTDIR)$(PREFIX)/lib/systemd/system/,$(UNITS))
	rm -f $(addprefix $(DESTDIR)$(PREFIX)/lib/systemd/system/multi-user.target.wants/,$(MULTI_USER_WANTS))
	rm -rf $(DESTDIR)$(PREFIX)/share/calamares/{branding/asahi,settings.conf,modules}

.PHONY: install uninstall
