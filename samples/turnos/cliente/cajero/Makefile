#!/usr/bin/make -f

C++C=cc -ggdb
OBJECTS=cajero.o classapp.o

INCLUDE=
LIB=-lstdc++ -lgmc
DEFINE=-D N_DEBUG

.cc.o:
	$(C++C) $(INCLUDE) $(DEFINE) -c $? -o $*.o

all: cajero

cajero: $(OBJECTS)
	$(C++C) $(LIB) -O $(OBJECTS) -o cajero

clean:
	rm -f $(OBJECTS) cajero

