#!/usr/bin/make -f

INCLUDE=
LIB=-lgmqw -lstdc++
C++C=cc
OBJECTS=statups.o wpower.o
OUTPUT=statups
COMP_OPTIONS=-D N_DEBUG
LINK_OPTIONS=

.cc.o:
	$(C++C) $(INCLUDE) $(COMP_OPTIONS) -c $?

all: statups

statups: $(OBJECTS)
	$(C++C) $(LINK_OPTIONS) $(LIB) -O $(OBJECTS) -o $(OUTPUT)

clean:
	rm -f $(OBJECTS) $(OUTPUT)

install:
	cp -f statups /usr/local/sbin/
	./update-tables.sh . /var/lib/gmonitor server /usr/local/sbin/statups
	./update-tables.sh . /var/lib/gmonitor funcion


uninstall:
	rm -f /usr/local/sbin/statups
