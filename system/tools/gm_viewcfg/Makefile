#!/usr/bin/make -f

include ../../configure.mk

CC=g++
INCLUDE=-I./ -I../../shared -I../../client
DEFINE=-D __NO__DEBUG__ -DBIN_MONITOR=$(BIN_MONITOR) -DMONITOR_CONFIG_PATH=$(MONITOR_CONFIG_PATH) \
	-DLOG_FILES=$(LOG_FILES) -DMAX_SERVERS=$(MAX_SERVERS) -DMAX_SERVICES=$(MAX_SERVICES)
CFLAGS=
CXXFLAGS=-g -Wall -Wextra -MMD -O2 
LFLAGS=-L../../client -L../../shared
LIBS=-lgmshared -lgmc -lssl -lcrypto

OBJECTS=gm_viewcfg.o
OUTPUT=gm_viewcfg

all: gm_viewcfg

gm_viewcfg.o: gm_viewcfg.cc
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c gm_viewcfg.cc

gm_viewcfg: $(OBJECTS)
	$(CC) $(CXXFLAGS) $(LFLAGS) -O $(OBJECTS) $(LIBS) -o $(OUTPUT) 

clean:
	$(RM)  $(OBJECTS) $(OUTPUT) *.d

install:
	$(CP) $(OUTPUT) $(INST_BINDIR)/
	chown $(INST_USER): $(INST_BINDIR)/$(OUTPUT)
	chmod 0755 $(INST_BINDIR)/$(OUTPUT)

uninstall:
	$(RM) $(INST_BINDIR)/$(OUTPUT)
