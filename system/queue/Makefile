#!/usr/bin/make -f

include ../configure.mk

INCLUDE=-I./ -I../shared
DEFINE=$(GENERAL_DEFINES)
CFLAGS=-fPIC
CXXFLAGS=$(PROCESSOR-PARAMS) -g -Wall -Wextra -MMD -O2 
LFLAGS=-L../shared/$(PROG)
LIBS=-lgmshared -lssl -lcrypto

OBJECTS_GMQ=$(OBJ)/gmsbase.o $(OBJ)/gmq.o $(OBJ)/gms.o
OUTPUT_GMQ=$(PROG)/$(EXE_GMQ)
EXE_GMQ=gmq

OBJECTS_LGMQ=$(OBJ)/gmsbase.o $(OBJ)/gmq.o
OUTPUT_LGMQ=$(PROG)/$(EXE_LGMQ)
EXE_LGMQ=libgmq.so

OBJECTS_LGMQW=$(OBJ)/gmsbase.o $(OBJ)/gmswaited.o
OUTPUT_LGMQW=$(PROG)/$(EXE_LGMQW)
EXE_LGMQW=libgmqw.so

all: $(OUTPUT_GMQ) $(OUTPUT_LGMQ) $(OUTPUT_LGMQW) 

$(OBJ)/gmsbase.o: gmsbase.cc gmsbase.h
	$(MKDIR) $(OBJ)
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c gmsbase.cc -o $(OBJ)/gmsbase.o
$(OBJ)/gmswaited.o: gmswaited.cc gmswaited.h
	$(MKDIR) $(OBJ)
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c gmswaited.cc -o $(OBJ)/gmswaited.o
$(OBJ)/gmq.o: gmq.cc
	$(MKDIR) $(OBJ)
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c gmq.cc -o $(OBJ)/gmq.o
$(OBJ)/gms.o: gms.cc gms.h
	$(MKDIR) $(OBJ)
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c gms.cc -o $(OBJ)/gms.o

$(OUTPUT_GMQ): $(OBJECTS_GMQ)
	$(MKDIR) $(PROG)
	$(CC) $(CXXFLAGS) $(LFLAGS) -O $(OBJECTS_GMQ) $(LIBS) -o $(OUTPUT_GMQ) 

$(OUTPUT_LGMQ): $(OBJECTS_LGMQ)
	$(MKDIR) $(PROG)
	$(CC) $(CXXFLAGS) $(LFLAGS) -O $(OBJECTS_LGMQ) $(LIBS) -shared -o $(OUTPUT_LGMQ) 

$(OUTPUT_LGMQW): $(OBJECTS_LGMQW)
	$(MKDIR) $(PROG)
	$(CC) $(CXXFLAGS) $(LFLAGS) -O $(OBJECTS_LGMQW) $(LIBS) -shared -o $(OUTPUT_LGMQW) 

clean:
	$(RMR) $(MACHINE)

install:
	mkdir -p $(INST_LOGDIR)
	$(CP) $(OUTPUT_GMQ) $(INST_SBINDIR)/
	chown $(INST_USER): $(INST_SBINDIR)/$(EXE_GMQ)
	chmod 0755 $(INST_SBINDIR)/$(EXE_GMQ)
	$(CP) $(OUTPUT_LGMQ) $(INST_LIBDIR)/
	chown $(INST_USER): $(INST_LIBDIR)/$(EXE_LGMQ)
	chmod 0755 $(INST_LIBDIR)/$(EXE_LGMQ)
	$(CP) $(OUTPUT_LGMQW) $(INST_LIBDIR)/
	chown $(INST_USER): $(INST_LIBDIR)/$(EXE_LGMQW)
	chmod 0755 $(INST_LIBDIR)/$(EXE_LGMQW)
	$(CP) gmsbase.h $(INST_INCDIR)/
	$(CP) gms.h $(INST_INCDIR)/
	$(CP) gmswaited.h $(INST_INCDIR)/

uninstall:
	$(RM) $(INST_SBINDIR)/$(EXE_GMQ)
	$(RM) $(INST_LIBDIR)/$(EXE_LGMQ)
	$(RM) $(INST_LIBDIR)/$(EXE_LGMQW)
	$(RM) $(INST_INCDIR)/gmsbase.h
	$(RM) $(INST_INCDIR)/gms.h
	$(RM) $(INST_INCDIR)/gmswaited.h
