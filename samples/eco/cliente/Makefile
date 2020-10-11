#!/usr/bin/make -f

C++C=cc -ggdb
OBJECTS=gmctest.o classapp.o

INCLUDE=
LIB=-lstdc++ -lgmc
DEFINE=-D N_DEBUG

.cc.o:
	$(C++C) $(INCLUDE) $(DEFINE) -c $? -o $*.o

all: gmctest

gmctest: $(OBJECTS)
	$(C++C) $(LIB) -O $(OBJECTS) -o gmctest

clean:
	rm -f $(OBJECTS) gmctest

