#!/usr/bin/make -f

include ../../configure.mk

CC=g++
INCLUDE=-I./ -I../../shared  -I../../queue
DEFINE=$(GENERAL_DEFINES)
CFLAGS=
CXXFLAGS=$(PROCESSOR-PARAMS) -g -Wall -Wextra -MMD -O2 
LFLAGS=-L../../shared -L../../queue 
LIBS=-lgmshared -lgmq

OBJECTS=gm_config.o gmconfig.o
OUTPUT=gm_config

all: gm_config

gmconfig.o: gmconfig.cc
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c gmconfig.cc

gm_config.o: gm_config.cc
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c gm_config.cc

gm_config: $(OBJECTS)
	$(CC) $(CXXFLAGS) $(LFLAGS) -O $(OBJECTS) $(LIBS) -o $(OUTPUT) 

clean:
	$(RM)  $(OBJECTS) $(OUTPUT) *.d

install:
	$(MKDIR) $(INST_LOGDIR)
	$(CP) $(OUTPUT) $(INST_SBINDIR)/
	chown $(INST_USER): $(INST_SBINDIR)/$(OUTPUT)
	chmod 0755 $(INST_SBINDIR)/$(OUTPUT)
	chmod 0755 update-tables.sh
	./update-tables.sh $(INST_VARDIR) server $(INST_SBINDIR)/$(OUTPUT)
	./update-tables.sh $(INST_VARDIR) funcion

uninstall:
	$(RM) $(INST_SBINDIR)/$(OUTPUT)
