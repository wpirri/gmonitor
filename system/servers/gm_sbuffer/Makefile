#!/usr/bin/make -f

include ../../configure.mk

INCLUDE=-I./ -I../../shared  -I../../queue
DEFINE=$(GENERAL_DEFINES)
CFLAGS=
CXXFLAGS=$(PROCESSOR-PARAMS) -g -Wall -Wextra -MMD -O2 
LFLAGS=-L../../shared/$(PROG) -L../../queue/$(PROG)
LIBS=-lgmshared -lgmq
OBJECTS=$(OBJ)/gm_sbuffer.o $(OBJ)/gmsbuffer.o
OUTPUT=$(PROG)/$(EXE)
EXE=gm_sbuffer

all: $(OUTPUT)

$(OBJ)/gmsbuffer.o: gmsbuffer.cc
	$(MKDIR) $(OBJ)
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c gmsbuffer.cc -o $(OBJ)/gmsbuffer.o
$(OBJ)/gm_sbuffer.o: gm_sbuffer.cc
	$(MKDIR) $(OBJ)
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c gm_sbuffer.cc -o $(OBJ)/gm_sbuffer.o

$(OUTPUT): $(OBJECTS)
	$(MKDIR) $(PROG)
	$(CC) $(CXXFLAGS) $(LFLAGS) -O $(OBJECTS) $(LIBS) -o $(OUTPUT) 

clean:
	$(RMR) $(MACHINE)

install:
	$(MKDIR) $(INST_LOGDIR)
	$(CP) $(OUTPUT) $(INST_SBINDIR)/
	chown $(INST_USER): $(INST_SBINDIR)/$(EXE)
	chmod 0755 $(INST_SBINDIR)/$(EXE)
	chmod 0755 update-tables.sh
	./update-tables.sh $(INST_VARDIR) server $(INST_SBINDIR)/$(EXE)
	./update-tables.sh $(INST_VARDIR) funcion

uninstall:
	$(RM) $(INST_SBINDIR)/$(EXE)
