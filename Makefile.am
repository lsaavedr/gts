## Process this file with automake to produce Makefile.in

SUBDIRS = src tools examples doc test debian

OSC_DIR=$(HOME)/local/src/osc/home:popinet/$(PACKAGE)-snapshot

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = gts.pc

ChangeLog:
	darcs changes > ChangeLog

debian-snapshot: config.h
	darcs changes > ChangeLog
	sh debian/changelog.sh
	dpkg-buildpackage -rfakeroot -b
	rm -f debian/repo/*
	mv -f ../*.changes ../*.deb debian/repo
	cd debian/repo && dpkg-scanpackages . /dev/null | gzip -9c > Packages.gz

launchpad-snapshot: config.h dist
	tar xzf $(PACKAGE)-?.?.?.tar.gz
	cd $(PACKAGE)-?.?.? && ./configure && darcs changes > ChangeLog && sh debian/changelog.sh && dpkg-buildpackage -rfakeroot -k07AB22DC -S -sa
	rm -f $(PACKAGE)-?.?.?.tar.gz
	rm -r -f $(PACKAGE)-?.?.?

buildservice-snapshot: config.h dist
	tar xzf $(PACKAGE)-?.?.?.tar.gz
	rm -f $(PACKAGE)-?.?.?.tar.gz
	mv -f $(PACKAGE)-?.?.? $(PACKAGE)-snapshot
	cd $(PACKAGE)-snapshot && ./configure && darcs changes > ChangeLog && sh debian/changelog.sh && chmod +x debian/rules && make distclean
	tar czvf $(PACKAGE)-snapshot.tar.gz $(PACKAGE)-snapshot
	dpkg-source -b $(PACKAGE)-snapshot
	rm -r -f $(PACKAGE)-snapshot
	cd $(OSC_DIR) && osc update && rm -f $(OSC_DIR)/*
	cp -f rpm/$(PACKAGE).spec $(OSC_DIR)
	mv -f $(PACKAGE)-snapshot.tar.gz $(PACKAGE)-snapshot_*.tar.gz $(PACKAGE)-snapshot_*.dsc $(OSC_DIR)
	cd $(OSC_DIR) && osc addremove && osc commit -m "snapshot release"
