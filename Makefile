PREFIX=/usr/local

SCRIPTS=bin/setup.sh
UNITS=calamares-firstboot.service
MULTI_USER_WANTS=calamares-firstboot.service

install:
	install -d $(DESTDIR)$(PREFIX)/libexec/calamares-firstboot
	install -m0755 -t $(DESTDIR)$(PREFIX)/libexec/calamares-firstboot $(SCRIPTS)
	install -dD $(DESTDIR)$(PREFIX)/lib/systemd/system
	install -m0644 -t $(DESTDIR)$(PREFIX)/lib/systemd/system $(addprefix systemd/,$(UNITS))
	install -d $(DESTDIR)$(PREFIX)/share/calamares/
	cp -r calamares/* $(DESTDIR)$(PREFIX)/share/calamares/

uninstall:
	rm -f $(addprefix $(DESTDIR)$(PREFIX)/libexec/,$(SCRIPTS))
	rm -f $(addprefix $(DESTDIR)$(PREFIX)/lib/systemd/system/,$(UNITS))
	rm -f $(addprefix $(DESTDIR)$(PREFIX)/lib/systemd/system/multi-user.target.wants/,$(MULTI_USER_WANTS))
	rm -rf $(DESTDIR)$(PREFIX)/share/calamares/{branding/asahi,settings.conf,modules}

.PHONY: install uninstall
