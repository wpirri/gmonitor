
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
GM_COMM_MSG_LEN=10240
MAX_SERVERS=32
MAX_SERVICES=256

CP=cp
CPUVA=cp -uva
RM=rm  -f
RMR=rm -rf
MKDIR=mkdir -p
CHMOD=chmod

# RBPi2 - -mcpu=cortex-a7 -mfloat-abi=hard -mfpu=neon-vfpv4
# RBPi3 - -mcpu=cortex-a53 -mfloat-abi=hard -mfpu=neon-fp-armv8 -mneon-for-64bits
# RBPi4 - -mcpu=cortex-a72 -mfloat-abi=hard -mfpu=neon-fp-armv8 -mneon-for-64bits
# Habilitar la siguiente definicion si se compila en RBPi3 o 4 para compatibilidad
# con RBPi2
#PROCESSOR-PARAMS=-mcpu=cortex-a7 -mfloat-abi=hard -mfpu=neon-vfpv4
