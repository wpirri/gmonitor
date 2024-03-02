#!/usr/bin/make -f

include ../../configure.mk

CC=g++
INCLUDE=-I./ -I../../shared -I../../client
DEFINE=$(GENERAL_DEFINES)
CFLAGS=
CXXFLAGS=$(PROCESSOR-PARAMS) -g -Wall -Wextra -MMD -O2 
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
