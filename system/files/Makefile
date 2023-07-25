#!/usr/bin/make -f

install:
	make -C script install
	make -C sysconfig install
	make -C tables install
	make -C xinetd.d install

uninstall:
	make -C script uninstall
	make -C sysconfig uninstall
	make -C tables uninstall
	make -C xinetd.d uninstall
