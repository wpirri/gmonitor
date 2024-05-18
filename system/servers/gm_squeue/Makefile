#!/usr/bin/make -f

include ../../configure.mk

INCLUDE=-I./ -I../../shared  -I../../queue
DEFINE=$(GENERAL_DEFINES)
CFLAGS=
CXXFLAGS=$(PROCESSOR-PARAMS) -g -Wall -Wextra -MMD -O2 
LFLAGS=-L../../shared/$(PROG) -L../../queue/$(PROG)
LIBS=-lgmshared -lgmq
OBJECTS=$(OBJ)/gmsafile.o $(OBJ)/gmsaf.o $(OBJ)/gm_squeue.o
OUTPUT=$(PROG)/$(EXE)
EXE=gm_squeue

all: $(OUTPUT)

$(OBJ)/gmsaf.o: gmsaf.cc gmsaf.h
	$(MKDIR) $(OBJ)
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c gmsaf.cc -o $(OBJ)/gmsaf.o
$(OBJ)/gmsafile.o: gmsafile.cc gmsafile.h
	$(MKDIR) $(OBJ)
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c gmsafile.cc -o $(OBJ)/gmsafile.o
$(OBJ)/gm_squeue.o: gm_squeue.cc
	$(MKDIR) $(OBJ)
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c gm_squeue.cc -o $(OBJ)/gm_squeue.o

$(OUTPUT): $(OBJECTS)
	$(MKDIR) $(PROG)
	$(CC) $(CXXFLAGS) $(LFLAGS) -O $(OBJECTS) $(LIBS) -o $(OUTPUT) 

clean:
	$(RMR) $(MACHINE)

install:
	mkdir -p $(INST_LOGDIR)
	mkdir -p $(INST_VARDIR)/saf
	chmod 0777 $(INST_VARDIR)/saf
	$(CP) $(OUTPUT) $(INST_SBINDIR)/
	chown $(INST_USER): $(INST_SBINDIR)/$(EXE)
	chmod 0755 $(INST_SBINDIR)/$(EXE)
	chmod 0755 update-tables.sh
	./update-tables.sh $(INST_VARDIR) server $(INST_SBINDIR)/$(EXE)
	./update-tables.sh $(INST_VARDIR) funcion

uninstall:
	$(RM) $(INST_SBINDIR)/$(EXE)
