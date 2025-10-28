#!/usr/bin/make -f

include ../configure.mk

INCLUDE=-I./
DEFINE=$(GENERAL_DEFINES)
CXXFLAGS=$(PROCESSOR-PARAMS) -g -Wall -Wextra -MMD -O2 
LFLAGS=
LIBS=
CFLAGS=-fPIC
OBJECTS=$(OBJ)/cmsg.o $(OBJ)/glog.o $(OBJ)/gmcomm.o $(OBJ)/gmessage.o $(OBJ)/gmidat.o $(OBJ)/gmontdb.o \
	$(OBJ)/shmem.o $(OBJ)/sincro.o $(OBJ)/ctcp.o $(OBJ)/gmbuffer.o $(OBJ)/gmerror.o $(OBJ)/gmheader.o \
	$(OBJ)/gmisc.o $(OBJ)/gmstring.o $(OBJ)/sincmem.o
EXE=libgmshared.so
OUTPUT=$(PROG)/$(EXE)
HEADER_FILES=cmsg.h glog.h gmcomm.h gmerror.h gmheader.h gmisc.h gmshared.h msgtype.h sincmem.h svcstru.h \
			ctcp.h gmbuffer.h gmconst.h gmessage.h gmidat.h gmontdb.h gmstring.h shmem.h sincro.h

all: $(OUTPUT)

$(OBJ)/glog.o: glog.cc glog.h
	$(MKDIR) $(OBJ)
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c glog.cc -o $(OBJ)/glog.o
$(OBJ)/ctcp.o: ctcp.cc ctcp.h
	$(MKDIR) $(OBJ)
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c ctcp.cc -o $(OBJ)/ctcp.o
$(OBJ)/gmbuffer.o: gmbuffer.cc gmbuffer.h
	$(MKDIR) $(OBJ)
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c gmbuffer.cc -o $(OBJ)/gmbuffer.o
$(OBJ)/gmerror.o: gmerror.cc gmerror.h
	$(MKDIR) $(OBJ)
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c gmerror.cc -o $(OBJ)/gmerror.o
$(OBJ)/gmidat.o: gmidat.cc gmidat.h
	$(MKDIR) $(OBJ)
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c gmidat.cc -o $(OBJ)/gmidat.o
$(OBJ)/gmheader.o: gmheader.cc gmheader.h
	$(MKDIR) $(OBJ)
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c gmheader.cc -o $(OBJ)/gmheader.o
$(OBJ)/gmessage.o: gmessage.cc gmessage.h
	$(MKDIR) $(OBJ)
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c gmessage.cc -o $(OBJ)/gmessage.o
$(OBJ)/gmcomm.o: gmcomm.cc gmcomm.h
	$(MKDIR) $(OBJ)
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c gmcomm.cc -o $(OBJ)/gmcomm.o
$(OBJ)/cmsg.o: cmsg.cc cmsg.h
	$(MKDIR) $(OBJ)
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c cmsg.cc -o $(OBJ)/cmsg.o
$(OBJ)/shmem.o: shmem.cc shmem.h
	$(MKDIR) $(OBJ)
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c shmem.cc -o $(OBJ)/shmem.o
$(OBJ)/sincro.o: sincro.cc sincro.h
	$(MKDIR) $(OBJ)
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c sincro.cc -o $(OBJ)/sincro.o
$(OBJ)/sincmem.o: sincmem.cc sincmem.h
	$(MKDIR) $(OBJ)
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c sincmem.cc -o $(OBJ)/sincmem.o
$(OBJ)/gmontdb.o: gmontdb.cc gmontdb.h
	$(MKDIR) $(OBJ)
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c gmontdb.cc -o $(OBJ)/gmontdb.o
$(OBJ)/gmisc.o: gmisc.cc gmisc.h
	$(MKDIR) $(OBJ)
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c gmisc.cc -o $(OBJ)/gmisc.o
$(OBJ)/gmstring.o: gmstring.cc gmstring.h
	$(MKDIR) $(OBJ)
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c gmstring.cc -o $(OBJ)/gmstring.o

$(OUTPUT): $(OBJECTS)
	$(MKDIR) $(PROG)
	$(CC) $(CXXFLAGS) $(LFLAGS) -O $(OBJECTS) $(LIBS) -shared -o $(OUTPUT) 

clean:
	$(RMR) $(MACHINE)

install:
	$(MKDIR) $(INST_LIBDIR)
	$(CP) $(OUTPUT) $(INST_LIBDIR)/
	chown $(INST_USER): $(INST_LIBDIR)/$(EXE)
	chmod 0755 $(INST_LIBDIR)/$(EXE)
	$(MKDIR) $(INST_HEADDIR)
	$(CP) $(HEADER_FILES) $(INST_HEADDIR)/

uninstall:
	$(RM) $(INST_LIBDIR)/$(EXE)
	cd $(INST_HEADDIR); $(RM) $(HEADER_FILES); cd -
