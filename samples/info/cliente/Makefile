#!/usr/bin/make -f

C++C=cc -ggdb
OBJECTS=gminfo.o classapp.o

INCLUDE=
LIB=-lstdc++ -lgmc
DEFINE=-D N_DEBUG

.cc.o:
	$(C++C) $(INCLUDE) $(DEFINE) -c $? -o $*.o

all: gmctest

gmctest: $(OBJECTS)
	$(C++C) $(LIB) -O $(OBJECTS) -o gminfo.cgi

clean:
	rm -f $(OBJECTS) gminfo.cgi

