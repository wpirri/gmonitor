#!/usr/bin/make -f

include ../../configure.mk

install:
	$(CP) clean-gmonitor-logs /etc/cron.daily/
	chmod 0755 /etc/cron.daily/clean-gmonitor-logs

	$(CP) clean-gmonitor-old-logs.sh $(INST_SBINDIR)/
	chmod 0755 $(INST_SBINDIR)/clean-gmonitor-old-logs.sh
	$(CP) gmon_config $(INST_SBINDIR)/
	chmod 0755 $(INST_SBINDIR)/gmon_config
	$(CP) gmon_start $(INST_SBINDIR)/
	chmod 0755 $(INST_SBINDIR)/gmon_start
	$(CP) gmon_status $(INST_SBINDIR)/
	chmod 0755 $(INST_SBINDIR)/gmon_status
	$(CP) gmon_stop $(INST_SBINDIR)/
	chmod 0755 $(INST_SBINDIR)/gmon_stop
	$(CP) gmond $(INST_SBINDIR)/
	chmod 0755 $(INST_SBINDIR)/gmond

	if test -d /etc/init.d; then\
	  ln -sf $(INST_SBINDIR)/gmond /etc/init.d/gmond;\
	else\
	  if test -d /etc/rc.d; then\
	    ln -sf $(INST_SBINDIR)/gmond /etc/rc.d/gmond;\
	  fi;\
	fi

uninstall:
	$(RM) /etc/init.d/gmond
	$(RM) /etc/rc.d/gmond
	$(RM) /etc/cron.daily/clean-gmonitor-logs
	$(RM) $(INST_SBINDIR)/clean-gmonitor-old-logs.sh
	$(RM) $(INST_SBINDIR)/gmon_config
	$(RM) $(INST_SBINDIR)/gmon_start
	$(RM) $(INST_SBINDIR)/gmon_status
	$(RM) $(INST_SBINDIR)/gmon_stop
	$(RM) $(INST_SBINDIR)/gmond
	