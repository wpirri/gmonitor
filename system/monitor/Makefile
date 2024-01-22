#!/usr/bin/make -f

include ../configure.mk

CC=g++
INCLUDE=-I./ -I../shared
DEFINE=-D __NO__DEBUG__ -DBIN_MONITOR=$(BIN_MONITOR) -DGM_COMM_MSG_LEN=$(GM_COMM_MSG_LEN) \
	-DMONITOR_CONFIG_PATH=$(MONITOR_CONFIG_PATH) -DLOG_FILES=$(LOG_FILES) -DMAX_SERVERS=$(MAX_SERVERS) \
	-DMAX_SERVICES=$(MAX_SERVICES)
CFLAGS=
CXXFLAGS=$(PROCESSOR-PARAMS) -g -Wall -Wextra -MMD -O2 
LFLAGS=-L../shared
LIBS=-lgmshared -lssl -lcrypto

OBJECTS=monitor.o
OUTPUT=monitor

all: monitor

monitor.o: monitor.cc
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c monitor.cc

monitor: $(OBJECTS)
	$(CC) $(CXXFLAGS) $(LFLAGS) -O $(OBJECTS) $(LIBS) -o $(OUTPUT) 

clean:
	$(RM)  $(OBJECTS) $(OUTPUT) *.d

install:
	$(CP) $(OUTPUT) $(INST_SBINDIR)/
	chown $(INST_USER): $(INST_SBINDIR)/$(OUTPUT)
	chmod 0755 $(INST_SBINDIR)/$(OUTPUT)

uninstall:
	$(RM) $(INST_SBINDIR)/$(OUTPUT)
