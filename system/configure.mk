
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
INST_INCDIR=/usr/local/include/gmonitor
INST_LOGDIR=/var/log/gmonitor

BIN_MONITOR=\"$(INST_SBINDIR)/gmt\"
MONITOR_CONFIG_PATH=\"$(INST_VARDIR)\"
LOG_FILES=\"$(INST_LOGDIR)\"
SAF_FILES=\"$(INST_VARDIR)/saf\"

GM_CONFIG_KEY=0x131313
GM_COMM_MSG_LEN=32768
MAX_SERVERS=32
MAX_SERVICES=256

MACHINE=.p$(shell uname -m)

OBJ=$(MACHINE)/obj
PROG=$(MACHINE)/exe
INST=$(MACHINE)/inst

CC=g++
CP=cp
CPUVA=cp -uva
RM=rm  -f
RMR=rm -rf
MKDIR=mkdir -p
CHMOD=chmod

GENERAL_DEFINES=-D __NO__DEBUG__ -DLOG_FILES=$(LOG_FILES) -DBIN_MONITOR=$(BIN_MONITOR) \
	-DMAX_SERVERS=$(MAX_SERVERS) -DMAX_SERVICES=$(MAX_SERVICES) -DSAF_FILES=$(SAF_FILES) \
	-DGM_COMM_MSG_LEN=$(GM_COMM_MSG_LEN) -DMONITOR_CONFIG_PATH=$(MONITOR_CONFIG_PATH) \
	-DGM_CONFIG_KEY=$(GM_CONFIG_KEY)

# RBPi2 - -mcpu=cortex-a7 -mfloat-abi=hard -mfpu=neon-vfpv4
# RBPi3 - -mcpu=cortex-a53 -mfloat-abi=hard -mfpu=neon-fp-armv8 -mneon-for-64bits
# RBPi4 - -mcpu=cortex-a72 -mfloat-abi=hard -mfpu=neon-fp-armv8 -mneon-for-64bits
# Habilitar la siguiente definicion si se compila en RBPi3 o 4 para compatibilidad
# con RBPi2
#PROCESSOR-PARAMS-ARM=-mcpu=cortex-a7 -mfloat-abi=hard -mfpu=neon-vfpv4
