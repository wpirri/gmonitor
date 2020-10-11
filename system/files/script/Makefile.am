gmon_config_SCRIPTS = gmon_config gmond gmon_start gmon_status gmon_stop 

install-data-local:
	if test -d /etc/init.d; then\
	  ln -sf $(sbindir)/gmond /etc/init.d/gmond;\
	else\
	  if test -d /etc/rc.d; then\
	    ln -sf $(sbindir)/gmond /etc/rc.d/gmond;\
	  fi;\
	fi
uninstall-local:
	rm -f /etc/init.d/gmond
	rm -f /etc/rc.d/gmond
