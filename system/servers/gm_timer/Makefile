#!/usr/bin/make -f

include ../../configure.mk

INCLUDE=-I./ -I../../shared  -I../../queue
DEFINE=$(GENERAL_DEFINES)
CXXFLAGS=$(PROCESSOR-PARAMS) -g -Wall -Wextra -MMD -O2 
LFLAGS=-L../../shared/$(PROG) -L../../queue/$(PROG)
LIBS=-lgmshared -lgmqw
OBJECTS=$(OBJ)/gmtimer.o $(OBJ)/wtimer.o $(OBJ)/gm_timer.o
OUTPUT=$(PROG)/$(EXE)
EXE=gm_timer

all: $(OUTPUT)

$(OBJ)/wtimer.o: wtimer.cc wtimer.h
	$(MKDIR) $(OBJ)
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c wtimer.cc -o $(OBJ)/wtimer.o
$(OBJ)/gmtimer.o: gmtimer.cc gmtimer.h
	$(MKDIR) $(OBJ)
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c gmtimer.cc -o $(OBJ)/gmtimer.o
$(OBJ)/gm_timer.o: gm_timer.cc
	$(MKDIR) $(OBJ)
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c gm_timer.cc -o $(OBJ)/gm_timer.o

$(OUTPUT): $(OBJECTS)
	$(MKDIR) $(PROG)
	$(CC) $(CXXFLAGS) $(LFLAGS) -O $(OBJECTS) $(LIBS) -o $(OUTPUT) 

clean:
	$(RMR) $(MACHINE)

install:
	mkdir -p $(INST_LOGDIR)
	$(CP) $(OUTPUT) $(INST_SBINDIR)/
	chown $(INST_USER): $(INST_SBINDIR)/$(EXE)
	chmod 0755 $(INST_SBINDIR)/$(EXE)
	chmod 0755 update-tables.sh
	./update-tables.sh $(INST_VARDIR) server $(INST_SBINDIR)/$(EXE)
	./update-tables.sh $(INST_VARDIR) funcion

uninstall:
	$(RM) $(INST_SBINDIR)/$(EXE)
