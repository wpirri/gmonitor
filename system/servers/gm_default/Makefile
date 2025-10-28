#!/usr/bin/make -f

include ../../configure.mk

INCLUDE=-I./ -I../../shared  -I../../queue
DEFINE=$(GENERAL_DEFINES)
CFLAGS=
CXXFLAGS=$(PROCESSOR-PARAMS) -g -Wall -Wextra -MMD -O2 
LFLAGS=-L../../shared/$(PROG) -L../../queue/$(PROG)
LIBS=-lgmshared -lgmq
OBJECTS=$(OBJ)/gm_default.o
EXE=gm_default
OUTPUT=$(PROG)/$(EXE)

all: $(OUTPUT)

$(OBJ)/gm_default.o: gm_default.cc
	$(MKDIR) $(OBJ)
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c gm_default.cc -o $(OBJ)/gm_default.o

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
