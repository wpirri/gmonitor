#!/usr/bin/make -f

include ../configure.mk

INCLUDE=-I./
CFLAGS=-fPIC
CXXFLAGS=-g -Wall -Wextra -MMD -O2 
LIBS=
LFLAGS=-shared
OBJECTS=cmsg.o glog.o gmcomm.o gmessage.o gmidat.o gmontdb.o shmem.o sincro.o \
		ctcp.o gmbuffer.o gmerror.o gmheader.o gmisc.o gmstring.o sincmem.o


OUTPUT=libgmshared.so
DEFINE=-D __NO__DEBUG__ -DGM_CONFIG_KEY=$(GM_CONFIG_KEY)
CC=g++

HEADER_FILES=cmsg.h glog.h gmcomm.h gmerror.h gmheader.h gmisc.h gmshared.h msgtype.h sincmem.h svcstru.h \
			ctcp.h gmbuffer.h gmconst.h gmessage.h gmidat.h gmontdb.h gmstring.h shmem.h sincro.h

all: libgmshared.so

libgmshared.so: $(OBJECTS)
		$(CC) $(CXXFLAGS) $(LFLAGS) -O $(OBJECTS) $(LIBS) -o $(OUTPUT) 

glog.o: glog.cc glog.h
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c glog.cc

ctcp.o: ctcp.cc ctcp.h
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c ctcp.cc

gmbuffer.o: gmbuffer.cc gmbuffer.h
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c gmbuffer.cc

gmerror.o: gmerror.cc gmerror.h
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c gmerror.cc

gmidat.o: gmidat.cc gmidat.h
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c gmidat.cc

gmheader.o: gmheader.cc gmheader.h
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c gmheader.cc

gmessage.o: gmessage.cc gmessage.h
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c gmessage.cc

gmcomm.o: gmcomm.cc gmcomm.h
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c gmcomm.cc

cmsg.o: cmsg.cc cmsg.h
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c cmsg.cc

shmem.o: shmem.cc shmem.h
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c shmem.cc

sincro.o: sincro.cc sincro.h
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c sincro.cc

sincmem.o: sincmem.cc sincmem.h
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c sincmem.cc

gmontdb.o: gmontdb.cc gmontdb.h
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c gmontdb.cc

gmisc.o: gmisc.cc gmisc.h
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c gmisc.cc

gmstring.o: gmstring.cc gmstring.h
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c gmstring.cc

clean:
	$(RM) $(OBJECTS) $(OUTPUT) *.d

install:
	$(MKDIR) $(INST_LIBDIR)
	$(CP) $(OUTPUT) $(INST_LIBDIR)/
	chown $(INST_USER): $(INST_LIBDIR)/$(OUTPUT)
	chmod 0755 $(INST_LIBDIR)/$(OUTPUT)
	$(MKDIR) $(INST_HEADDIR)
	$(CP) $(HEADER_FILES) $(INST_HEADDIR)/

uninstall:
	$(RM) $(INST_LIBDIR)/$(OUTPUT)
	cd $(INST_HEADDIR); $(RM) $(HEADER_FILES); cd -
