#!/usr/bin/make -f

include ../configure.mk

CC=g++
INCLUDE=-I./ -I../shared
DEFINE=-D __NO__DEBUG__ -DBIN_MONITOR=$(BIN_MONITOR) -DGM_COMM_MSG_LEN=$(GM_COMM_MSG_LEN) 
CFLAGS=
CXXFLAGS=$(PROCESSOR-PARAMS) -g -Wall -Wextra -MMD -O2 
LFLAGS=-L../shared
LIBS=-lgmshared -lssl -lcrypto

OBJECTS=gmd.o
OUTPUT=gmd

all: gmd 

gmd.o: gmd.cc
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c gmd.cc

gmd: $(OBJECTS)
	$(CC) $(CXXFLAGS) $(LFLAGS) -O $(OBJECTS) $(LIBS) -o $(OUTPUT) 

clean:
	$(RM)  $(OBJECTS) $(OUTPUT) *.d

install:
	$(MKDIR) $(INST_LOGDIR)
	$(CP) $(OUTPUT) $(INST_SBINDIR)/
	chown $(INST_USER): $(INST_SBINDIR)/$(OUTPUT)
	chmod 0755 $(INST_SBINDIR)/$(OUTPUT)
uninstall:
	$(RM) $(INST_SBINDIR)/$(OUTPUT)
