
# sed -i 's/include "/include </g' *
# sed -i 's/\.h"/\.h>/g' *

RUN_USER=gmonitor
INST_USER=root
INST_LIBDIR=/usr/lib
INST_HEADDIR=/usr/local/include/gmonitor
INST_BINDIR=/usr/local/bin
INST_SBINDIR=/usr/local/sbin
INST_ETCDIR=/etc/gmonitor
INST_VARDIR=/var/lib/gmonitor
INST_INCDIR=/usr/local/include
INST_LOGDIR=/var/log/gmonitor

BIN_MONITOR=\"$(INST_BINDIR)/gmt\"
MONITOR_CONFIG_PATH=\"$(INST_VARDIR)\"
LOG_FILES=\"$(INST_LOGDIR)\"
SAF_FILES=\"$(INST_VARDIR)/saf\"

GM_CONFIG_KEY=0x131313
GM_COMM_MSG_LEN=10240
MAX_SERVERS=255
MAX_SERVICES=4096

CP=cp
RM=rm -f
MKDIR=mkdir -p
