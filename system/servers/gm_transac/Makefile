#!/usr/bin/make -f

include ../../configure.mk

CC=g++
INCLUDE=-I./ -I../../shared  -I../../queue
DEFINE=$(GENERAL_DEFINES)
CXXFLAGS=$(PROCESSOR-PARAMS) -g -Wall -Wextra -MMD -O2 
LFLAGS=-L../../shared -L../../queue 
LIBS=-lgmshared -lgmqw

OBJECTS=transac.o wtimer.o gm_transac.o
OUTPUT=gm_transac

all: gm_transac

wtimer.o: wtimer.cc wtimer.h
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c wtimer.cc

transac.o: transac.cc transac.h
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c transac.cc

gm_transac.o: gm_transac.cc
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c gm_transac.cc

gm_transac: $(OBJECTS)
	$(CC) $(CXXFLAGS) $(LFLAGS) -O $(OBJECTS) $(LIBS) -o $(OUTPUT) 

clean:
	$(RM)  $(OBJECTS) $(OUTPUT) *.d

install:
	mkdir -p $(INST_LOGDIR)
	$(CP) $(OUTPUT) $(INST_SBINDIR)/
	chown $(INST_USER): $(INST_SBINDIR)/$(OUTPUT)
	chmod 0755 $(INST_SBINDIR)/$(OUTPUT)
	chmod 0755 update-tables.sh
	./update-tables.sh $(INST_VARDIR) server $(INST_SBINDIR)/$(OUTPUT)
	./update-tables.sh $(INST_VARDIR) funcion

uninstall:
	$(RM) $(INST_SBINDIR)/$(OUTPUT)
