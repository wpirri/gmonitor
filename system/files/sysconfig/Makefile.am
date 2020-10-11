install-data-local:
	$(mkinstalldirs) $(sysconfdir)
	$(install_sh_DATA) gmonitor $(sysconfdir)/gmonitor
	echo "SERVER_CONF="$(gmon_tbldir) >> $(sysconfdir)/gmonitor
	echo "SERVER_BIN="$(sbindir) >> $(sysconfdir)/gmonitor
	echo "CONFIG_PROMPT=\"gmonitor> \"" >> $(sysconfdir)/gmonitor
	x=`uname`; \
	if test $$x = "Linux"; then \
	  echo "PS_EAF=\"ps -eaf\"" >> $(sysconfdir)/gmonitor ;\
	else \
	  echo "PS_EAF=\"ps -af\"" >> $(sysconfdir)/gmonitor ;\
	fi;
uninstall-local:
	rm -f $(sysconfdir)/gmonitor
