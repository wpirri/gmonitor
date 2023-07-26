#!/usr/bin/make -f

include ../configure.mk

CC=g++
INCLUDE=-I./ -I../shared
DEFINE=-D __NO__DEBUG__ -DGM_COMM_MSG_LEN=$(GM_COMM_MSG_LEN)
CFLAGS=-fPIC
CXXFLAGS=-g -Wall -Wextra -MMD -O2 
LFLAGS=-L../shared -shared
LIBS=-lgmshared -lssl -lcrypto

OBJECTS=gmc.o
OUTPUT=libgmc.so

all: libgmc.so

gmc.o: gmc.cc
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c gmc.cc

libgmc.so: $(OBJECTS)
	$(CC) $(CXXFLAGS) $(LFLAGS) -O $(OBJECTS) $(LIBS) -o $(OUTPUT) 

clean:
	$(RM)  $(OBJECTS) $(OUTPUT) *.d

install:
	$(MKDIR) $(INST_LIBDIR)
	$(CP) $(OUTPUT) $(INST_LIBDIR)/
	chown $(INST_USER): $(INST_LIBDIR)/$(OUTPUT)
	chmod 0755 $(INST_LIBDIR)/$(OUTPUT)
	$(CP) gmc.h $(INST_INCDIR)/

uninstall:
	$(RM) $(INST_LIBDIR)/$(OUTPUT)
	$(RM) $(INST_INCDIR)/gmc.h
