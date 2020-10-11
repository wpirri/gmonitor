SUBDIRS = shared listener client router queue files tools servers
ACLOCAL_AMFLAGS = -I m4

build-listener:
	(cd shared   && $(MAKE) ) || exit 1
	(cd listener && $(MAKE) ) || exit 1
build-monitor:
	(cd shared   && $(MAKE) ) || exit 1
	(cd listener && $(MAKE) ) || exit 1
	(cd router   && $(MAKE) ) || exit 1
	(cd queue    && $(MAKE) ) || exit 1
build-server:
	(cd shared   && $(MAKE) ) || exit 1
	(cd queue    && $(MAKE) ) || exit 1
	(cd servers  && $(MAKE) ) || exit 1
build-client:
	(cd shared   && $(MAKE) ) || exit 1
	(cd client   && $(MAKE) ) || exit 1
build-tools:
	(cd shared   && $(MAKE) ) || exit 1
	(cd tools    && $(MAKE) ) || exit 1

install-listener:
	(cd shared   && $(MAKE) install ) || exit 1
	(cd listener && $(MAKE) install ) || exit 1
install-monitor:
	(cd shared   && $(MAKE) install ) || exit 1
	(cd listener && $(MAKE) install ) || exit 1
	(cd router   && $(MAKE) install ) || exit 1
	(cd queue    && $(MAKE) install ) || exit 1
	(cd files    && $(MAKE) install ) || exit 1
	(cd tools    && $(MAKE) install ) || exit 1
install-server:
	(cd shared   && $(MAKE) install ) || exit 1
	(cd servers  && $(MAKE) install ) || exit 1
install-client:
	(cd shared   && $(MAKE) install ) || exit 1
	(cd client   && $(MAKE) install ) || exit 1
install-tools:
	(cd shared   && $(MAKE) install ) || exit 1
	(cd tools    && $(MAKE) install ) || exit 1

uninstall-listener:
	(cd listener && $(MAKE) uninstall ) || exit 1
uninstall-monitor:
	(cd listener && $(MAKE) uninstall ) || exit 1
	(cd router   && $(MAKE) uninstall ) || exit 1
	(cd queue    && $(MAKE) uninstall ) || exit 1
	(cd files    && $(MAKE) uninstall ) || exit 1
	(cd tools    && $(MAKE) uninstall ) || exit 1
uninstall-server:
	(cd servers  && $(MAKE) uninstall ) || exit 1
uninstall-client:
	(cd client   && $(MAKE) uninstall ) || exit 1
uninstall-tools:
	(cd tools    && $(MAKE) uninstall ) || exit 1

