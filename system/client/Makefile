#!/usr/bin/make -f

include ../configure.mk

INCLUDE=-I./ -I../shared
CXXFLAGS=$(PROCESSOR-PARAMS) -g -Wall -Wextra -MMD -O2 
LIBS=-lgmshared -lssl -lcrypto
DEFINE=$(GENERAL_DEFINES)
CFLAGS=-fPIC
LFLAGS=-L../shared/$(PROG) -shared
OBJECTS=$(OBJ)/gmc.o
EXE=libgmc.so
OUTPUT=$(PROG)/$(EXE)

all: $(OUTPUT)

$(OBJ)/gmc.o: gmc.cc
	$(MKDIR) $(OBJ)
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c gmc.cc -o $(OBJ)/gmc.o

$(OUTPUT): $(OBJECTS)
	$(MKDIR) $(PROG)
	$(CC) $(CXXFLAGS) $(LFLAGS) -O $(OBJECTS) $(LIBS) -o $(OUTPUT) 

clean:
	$(RMR) $(MACHINE)

install:
	$(MKDIR) $(INST_LIBDIR)
	$(CP) $(OUTPUT) $(INST_LIBDIR)/
	chown $(INST_USER): $(INST_LIBDIR)/$(EXE)
	chmod 0755 $(INST_LIBDIR)/$(EXE)
	$(CP) gmc.h $(INST_INCDIR)/

uninstall:
	$(RM) $(INST_LIBDIR)/$(EXE)
	$(RM) $(INST_INCDIR)/gmc.h
