#!/usr/bin/make -f

include ../../configure.mk

INCLUDE=-I./ -I../../shared -I../../client
DEFINE=$(GENERAL_DEFINES)
CFLAGS=
CXXFLAGS=$(PROCESSOR-PARAMS) -g -Wall -Wextra -MMD -O2 
LFLAGS=-L../../client/$(PROG) -L../../shared/$(PROG)
LIBS=-lgmshared -lgmc -lssl -lcrypto
OBJECTS=$(OBJ)/gm_viewcfg.o
OUTPUT=$(PROG)/$(EXE)
EXE=gm_viewcfg

all: $(OUTPUT)

$(OBJ)/gm_viewcfg.o: gm_viewcfg.cc
	$(MKDIR) $(OBJ)
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c gm_viewcfg.cc -o $(OBJ)/gm_viewcfg.o

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
