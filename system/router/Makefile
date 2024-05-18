#!/usr/bin/make -f

include ../configure.mk

INCLUDE=-I./ -I../shared
DEFINE=$(GENERAL_DEFINES)
CFLAGS=
CXXFLAGS=$(PROCESSOR-PARAMS) -g -Wall -Wextra -MMD -O2 
LFLAGS=-L../shared/$(PROG)
LIBS=-lgmshared -lssl -lcrypto
OBJECTS=$(OBJ)/gmt.o
OUTPUT=$(PROG)/$(EXE)
EXE=gmt

all: $(OUTPUT)

$(OBJ)/gmt.o: gmt.cc
	$(MKDIR) $(OBJ)
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c gmt.cc -o $(OBJ)/gmt.o

$(OUTPUT): $(OBJECTS)
	$(MKDIR) $(PROG)
	$(CC) $(CXXFLAGS) $(LFLAGS) -O $(OBJECTS) $(LIBS) -o $(OUTPUT) 

clean:
	$(RMR) $(MACHINE)

install:
	groupadd -f $(RUN_USER)
	useradd -c "Gnu Monitor" -d $(INST_VARDIR) -g $(RUN_USER) -s /bin/false $(RUN_USER) || echo "Ya existe el usuario"
	$(MKDIR) $(INST_LOGDIR)
	chown $(RUN_USER): $(INST_LOGDIR)

	$(MKDIR) $(INST_LOGDIR)
	$(CP) $(OUTPUT) $(INST_SBINDIR)/
	chown $(INST_USER): $(INST_SBINDIR)/$(EXE)
	chmod 0755 $(INST_SBINDIR)/$(EXE)

uninstall:
	$(RM) $(INST_SBINDIR)/$(EXE)
