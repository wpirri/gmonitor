#!/usr/bin/make -f

include ../../configure.mk

CC=g++
INCLUDE=-I./ -I../../shared -I../../client
DEFINE=-D __NO__DEBUG__
CFLAGS=
CXXFLAGS=$(PROCESSOR-PARAMS) -g -Wall -Wextra -MMD -O2 
LFLAGS=-L../../client -L../../shared
LIBS=-lgmshared -lgmc -lssl -lcrypto

OBJECTS=gm_safcfg.o
OUTPUT=gm_safcfg

all: gm_safcfg

gm_safcfg.o: gm_safcfg.cc
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c gm_safcfg.cc

gm_safcfg: $(OBJECTS)
	$(CC) $(CXXFLAGS) $(LFLAGS) -O $(OBJECTS) $(LIBS) -o $(OUTPUT) 

clean:
	$(RM)  $(OBJECTS) $(OUTPUT) *.d

install:
	$(CP) $(OUTPUT) $(INST_BINDIR)/
	chown $(INST_USER): $(INST_BINDIR)/$(OUTPUT)
	chmod 0755 $(INST_BINDIR)/$(OUTPUT)

uninstall:
	$(RM) $(INST_BINDIR)/$(OUTPUT)
