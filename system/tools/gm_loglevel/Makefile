#!/usr/bin/make -f

include ../../configure.mk

INCLUDE=-I./ -I../../shared -I../../client
DEFINE=$(GENERAL_DEFINES)
CFLAGS=
CXXFLAGS=$(PROCESSOR-PARAMS) -g -Wall -Wextra -MMD -O2 
LFLAGS=-L../../client/$(PROG) -L../../shared/$(PROG)
LIBS=-lgmshared -lgmc -lssl -lcrypto
OBJECTS=$(OBJ)/gm_loglevel.o $(OBJ)/classapp.o
OUTPUT=$(PROG)/$(EXE)
EXE=gm_loglevel

all: $(OUTPUT)

$(OBJ)/classapp.o: classapp.cc
	$(MKDIR) $(OBJ)
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c classapp.cc -o $(OBJ)/classapp.o
$(OBJ)/gm_loglevel.o: gm_loglevel.cc
	$(MKDIR) $(OBJ)
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c gm_loglevel.cc -o $(OBJ)/gm_loglevel.o

$(OUTPUT): $(OBJECTS)
	$(MKDIR) $(PROG)
	$(CC) $(CXXFLAGS) $(LFLAGS) -O $(OBJECTS) $(LIBS) -o $(OUTPUT) 

clean:
	$(RMR) $(MACHINE)

install:
	$(CP) $(OUTPUT) $(INST_BINDIR)/
	chown $(INST_USER): $(INST_BINDIR)/$(EXE)
	chmod 0755 $(INST_BINDIR)/$(EXE)

uninstall:
	$(RM) $(INST_BINDIR)/$(EXE)
