#!/usr/bin/make -f

include ../../configure.mk

OBJECTS=$(OBJ)/des.o $(OBJ)/sec.o
OUTPUT=$(PROG)/$(EXE)
EXE=gmcrypt

all: $(OUTPUT)

$(OBJ)/des.o:
	$(MKDIR) $(OBJ)
	$(CC) $(PROCESSOR-PARAMS) -c des.c -o $(OBJ)/des.o
$(OBJ)/sec.o:
	$(MKDIR) $(OBJ)
	$(CC) $(PROCESSOR-PARAMS) -c sec.c -o $(OBJ)/sec.o

$(OUTPUT): $(OBJECTS)
	$(MKDIR) $(PROG)
	$(CC) $(PROCESSOR-PARAMS) -O $(OBJECTS) -o $(OUTPUT) 

clean:
	$(RMR) $(MACHINE)

install:
	$(CP) $(OUTPUT) $(INST_BINDIR)/
	chown $(INST_USER): $(INST_BINDIR)/$(EXE)
	chmod 0755 $(INST_BINDIR)/$(EXE)

uninstall:
	$(RM) $(INST_BINDIR)/$(EXE)

