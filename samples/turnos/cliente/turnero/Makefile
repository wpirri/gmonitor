#!/usr/bin/make -f

C++C=cc -ggdb
OBJECTS=turnero.o classapp.o

INCLUDE=
LIB=-lstdc++ -lgmc
DEFINE=-D N_DEBUG

.cc.o:
	$(C++C) $(INCLUDE) $(DEFINE) -c $? -o $*.o

all: turnero

turnero: $(OBJECTS)
	$(C++C) $(LIB) -O $(OBJECTS) -o turnero

clean:
	rm -f $(OBJECTS) turnero

