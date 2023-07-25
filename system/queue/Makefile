#!/usr/bin/make -f

include ../configure.mk

CC=g++
INCLUDE=-I./ -I../shared
DEFINE=-D __NO__DEBUG__ -DBIN_MONITOR=$(BIN_MONITOR) -DMONITOR_CONFIG_PATH=$(MONITOR_CONFIG_PATH) -DLOG_FILES=$(LOG_FILES) -DMAX_SERVERS=$(MAX_SERVERS) -DMAX_SERVICES=$(MAX_SERVICES) 
CFLAGS=-fPIC
CXXFLAGS=-g -Wall -Wextra -MMD -O2 
LFLAGS=-L../shared -shared
LIBS=-lgmshared -lssl -lcrypto

OBJECTS_GMQ=gmsbase.o gmq.o gms.o
OUTPUT_GMQ=gmq

OBJECTS_LGMQ=gmsbase.o gmq.o
OUTPUT_LGMQ=libgmq.so

OBJECTS_LGMQW=gmsbase.o gmswaited.o
OUTPUT_LGMQW=libgmqw.so

all: gmq libgmq.so libgmqw.so 

gmsbase.o: gmsbase.cc gmsbase.h
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c gmsbase.cc

gmswaited.o: gmswaited.cc gmswaited.h
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c gmswaited.cc

gmq.o: gmq.cc
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c gmq.cc

gms.o: gms.cc gms.h
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c gms.cc

gmq: $(OBJECTS_GMQ)
	$(CC) $(CXXFLAGS) $(LFLAGS) -O $(OBJECTS_GMQ) $(LIBS) -o $(OUTPUT_GMQ) 

libgmq.so: $(OBJECTS_LGMQ)
	$(CC) $(CXXFLAGS) $(LFLAGS) -O $(OBJECTS_LGMQ) $(LIBS) -shared -o $(OUTPUT_LGMQ) 

libgmqw.so: $(OBJECTS_LGMQW)
	$(CC) $(CXXFLAGS) $(LFLAGS) -O $(OBJECTS_LGMQW) $(LIBS) -shared -o $(OUTPUT_LGMQW) 

clean:
	$(RM) $(OBJECTS_GMQ) $(OUTPUT_GMQ) $(OBJECTS_LGMQ) $(OUTPUT_LGMQ) $(OBJECTS_LGMQW) $(OUTPUT_LGMQW) *.d

install:
	mkdir -p $(INST_LOGDIR)
	$(CP) $(OUTPUT_GMQ) $(INST_SBINDIR)/
	chown $(INST_USER): $(INST_SBINDIR)/$(OUTPUT_GMQ)
	chmod 0755 $(INST_SBINDIR)/$(OUTPUT_GMQ)
	$(CP) $(OUTPUT_LGMQ) $(INST_LIBDIR)/
	chown $(INST_USER): $(INST_LIBDIR)/$(OUTPUT_LGMQ)
	chmod 0755 $(INST_LIBDIR)/$(OUTPUT_LGMQ)
	$(CP) $(OUTPUT_LGMQW) $(INST_LIBDIR)/
	chown $(INST_USER): $(INST_LIBDIR)/$(OUTPUT_LGMQW)
	chmod 0755 $(INST_LIBDIR)/$(OUTPUT_LGMQW)

uninstall:
	$(RM) $(INST_SBINDIR)/$(OUTPUT_GMQ)
	$(RM) $(INST_LIBDIR)/$(OUTPUT_LGMQ)
	$(RM) $(INST_LIBDIR)/$(OUTPUT_LGMQW)
