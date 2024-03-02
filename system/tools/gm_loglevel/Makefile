#!/usr/bin/make -f

include ../../configure.mk

CC=g++
INCLUDE=-I./ -I../../shared -I../../client
DEFINE=$(GENERAL_DEFINES)
CFLAGS=
CXXFLAGS=$(PROCESSOR-PARAMS) -g -Wall -Wextra -MMD -O2 
LFLAGS=-L../../client -L../../shared
LIBS=-lgmshared -lgmc -lssl -lcrypto

OBJECTS=gm_loglevel.o classapp.o
OUTPUT=gm_loglevel

all: gm_loglevel

classapp.o: classapp.cc
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c classapp.cc

gm_loglevel.o: gm_loglevel.cc
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c gm_loglevel.cc

gm_loglevel: $(OBJECTS)
	$(CC) $(CXXFLAGS) $(LFLAGS) -O $(OBJECTS) $(LIBS) -o $(OUTPUT) 

clean:
	$(RM)  $(OBJECTS) $(OUTPUT) *.d

install:
	$(CP) $(OUTPUT) $(INST_BINDIR)/
	chown $(INST_USER): $(INST_BINDIR)/$(OUTPUT)
	chmod 0755 $(INST_BINDIR)/$(OUTPUT)

uninstall:
	$(RM) $(INST_BINDIR)/$(OUTPUT)
