#!/usr/bin/make -f

include ../../configure.mk

install:
	$(MKDIR) $(INST_ETCDIR)
	$(CP) gmonitor $(INST_ETCDIR)/
	echo "SERVER_CONF="$(INST_VARDIR) >> $(INST_ETCDIR)/gmonitor
	echo "SERVER_BIN="$(INST_SBINDIR) >> $(INST_ETCDIR)/gmonitor
	echo "CLIENT_BIN="$(INST_BINDIR) >> $(INST_ETCDIR)/gmonitor
	echo "SU_USER=\"$(RUN_USER)\"" >> $(INST_ETCDIR)/gmonitor
	echo "CONFIG_PROMPT=\"gmonitor> \"" >> $(INST_ETCDIR)/gmonitor
	x=`uname`; \
	if test $$x = "Linux"; then \
	  echo "PS_EAF=\"ps -eaf\"" >> $(INST_ETCDIR)/gmonitor ;\
	else \
	  echo "PS_EAF=\"ps -af\"" >> $(INST_ETCDIR)/gmonitor ;\
	fi;

uninstall:
	$(RM) $(INST_ETCDIR)/gmonitor
