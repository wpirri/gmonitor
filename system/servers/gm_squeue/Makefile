#!/usr/bin/make -f

include ../../configure.mk

CC=g++
INCLUDE=-I./ -I../../shared  -I../../queue
DEFINE=-D __NO__DEBUG__ -DLOG_FILES=$(LOG_FILES) -DMAX_SERVERS=$(MAX_SERVERS) -DMAX_SERVICES=$(MAX_SERVICES) -DSAF_FILES=$(SAF_FILES)
CFLAGS=
CXXFLAGS=$(PROCESSOR-PARAMS) -g -Wall -Wextra -MMD -O2 
LFLAGS=-L../../shared -L../../queue 
LIBS=-lgmshared -lgmq

OBJECTS=gmsafile.o gmsaf.o gm_squeue.o
OUTPUT=gm_squeue

all: gm_squeue

gmsaf.o: gmsaf.cc gmsaf.h
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c gmsaf.cc

gmsafile.o: gmsafile.cc gmsafile.h
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c gmsafile.cc

gm_squeue.o: gm_squeue.cc
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c gm_squeue.cc

gm_squeue: $(OBJECTS)
	$(CC) $(CXXFLAGS) $(LFLAGS) -O $(OBJECTS) $(LIBS) -o $(OUTPUT) 

clean:
	$(RM)  $(OBJECTS) $(OUTPUT) *.d

install:
	mkdir -p $(INST_LOGDIR)
	mkdir -p $(INST_VARDIR)/saf
	chmod 0777 $(INST_VARDIR)/saf
	$(CP) $(OUTPUT) $(INST_SBINDIR)/
	chown $(INST_USER): $(INST_SBINDIR)/$(OUTPUT)
	chmod 0755 $(INST_SBINDIR)/$(OUTPUT)
	chmod 0755 update-tables.sh
	./update-tables.sh $(INST_VARDIR) server $(INST_SBINDIR)/$(OUTPUT)
	./update-tables.sh $(INST_VARDIR) funcion

uninstall:
	$(RM) $(INST_SBINDIR)/$(OUTPUT)
