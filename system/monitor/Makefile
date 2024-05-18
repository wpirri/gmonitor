#!/usr/bin/make -f

include ../configure.mk

INCLUDE=-I./ -I../shared
DEFINE=$(GENERAL_DEFINES)
CFLAGS=
CXXFLAGS=$(PROCESSOR-PARAMS) -g -Wall -Wextra -MMD -O2 
LFLAGS=-L../shared/$(PROG)
LIBS=-lgmshared -lssl -lcrypto

OBJECTS=monitor.o
OUTPUT=monitor

OBJECTS=$(OBJ)/monitor.o
OUTPUT=$(PROG)/$(EXE)
EXE=monitor

all: $(OUTPUT)

$(OBJ)/monitor.o: monitor.cc
	$(MKDIR) $(OBJ)
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c monitor.cc -o $(OBJ)/monitor.o

$(OUTPUT): $(OBJECTS)
	$(MKDIR) $(PROG)
	$(CC) $(CXXFLAGS) $(LFLAGS) -O $(OBJECTS) $(LIBS) -o $(OUTPUT) 

clean:
	$(RMR) $(MACHINE)

install:
	$(CP) $(OUTPUT) $(INST_SBINDIR)/
	chown $(INST_USER): $(INST_SBINDIR)/$(EXE)
	chmod 0755 $(INST_SBINDIR)/$(EXE)

uninstall:
	$(RM) $(INST_SBINDIR)/$(EXE)
