#!/usr/bin/make -f

all:
	make -C shared
	make -C listener
	make -C router
	make -C queue
	make -C client
	make -C servers
	make -C tools

clean:
	make -C shared clean
	make -C listener clean
	make -C router clean
	make -C queue clean
	make -C client clean
	make -C servers clean
	make -C tools clean

install:
	make -C files install
	make -C shared install
	make -C listener install
	make -C router install
	make -C queue install
	make -C client install
	make -C servers install
	make -C tools install

uninstall:
	make -C files uninstall
	make -C shared uninstall
	make -C listener uninstall
	make -C router uninstall
	make -C queue uninstall
	make -C client uninstall
	make -C servers uninstall
	make -C tools uninstall
