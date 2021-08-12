#!/usr/bin/make -f

CP=cp -a
RM=rm -f

STOP_SCRIPT=gmonitor-stop.sh
START_SCRIPT=gmonitor-start.sh
SCRIPT_PATH=/usr/sbin
SYSTEMD_SCRIPT=gmonitor.service
#SYSTEMD_PATH=/etc/systemd/system
SYSTEMD_PATH=/lib/systemd/system

install:
	$(CP) $(STOP_SCRIPT) $(SCRIPT_PATH)/$(STOP_SCRIPT)
	$(CP) $(START_SCRIPT) $(SCRIPT_PATH)/$(START_SCRIPT)
	$(CP) $(SYSTEMD_SCRIPT) $(SYSTEMD_PATH)/$(SYSTEMD_SCRIPT)
	systemctl daemon-reload
	systemctl enable $(SYSTEMD_SCRIPT)
	systemctl start $(SYSTEMD_SCRIPT)


uninstall:
	systemctl stop $(SYSTEMD_SCRIPT)
	systemctl disable $(SYSTEMD_SCRIPT)
	$(RM) $(SCRIPT_PATH)/$(STOP_SCRIPT)
	$(RM) $(SCRIPT_PATH)/$(START_SCRIPT)
	$(RM) $(SYSTEMD_PATH)/$(SYSTEMD_SCRIPT)
	systemctl daemon-reload

