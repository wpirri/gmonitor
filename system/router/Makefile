#!/usr/bin/make -f

include ../configure.mk

CC=g++
INCLUDE=-I./ -I../shared
DEFINE=$(GENERAL_DEFINES)
CFLAGS=
CXXFLAGS=$(PROCESSOR-PARAMS) -g -Wall -Wextra -MMD -O2 
LFLAGS=-L../shared
LIBS=-lgmshared -lssl -lcrypto

OBJECTS=gmt.o
OUTPUT=gmt

all: gmt

gmt.o: gmt.cc
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) $(DEFINE) -c gmt.cc

gmt: $(OBJECTS)
	$(CC) $(CXXFLAGS) $(LFLAGS) -O $(OBJECTS) $(LIBS) -o $(OUTPUT) 

clean:
	$(RM)  $(OBJECTS) $(OUTPUT) *.d

install:
	groupadd -f $(RUN_USER)
	useradd -c "Gnu Monitor" -d $(INST_VARDIR) -g $(RUN_USER) -s /bin/false $(RUN_USER) || echo "Ya existe el usuario"
	$(MKDIR) $(INST_LOGDIR)
	chown $(RUN_USER): $(INST_LOGDIR)

	$(MKDIR) $(INST_LOGDIR)
	$(CP) $(OUTPUT) $(INST_SBINDIR)/
	chown $(INST_USER): $(INST_SBINDIR)/$(OUTPUT)
	chmod 0755 $(INST_SBINDIR)/$(OUTPUT)

uninstall:
	$(RM) $(INST_SBINDIR)/$(OUTPUT)
