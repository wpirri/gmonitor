#!/usr/bin/make -f

INCLUDE=
LIB=-lgmc -lstdc++
C++C=cc
OBJECTS=ups.o
OUTPUT=upsd
DEFINE=-D N_DEBUG

.cc.o:
	$(C++C) $(INCLUDE) $(DEFINE) -c $?

all: upsd

upsd: $(OBJECTS)
	$(C++C) $(LIB) -O $(OBJECTS) -o $(OUTPUT) 

clean:
	rm -f $(OBJECTS) $(OUTPUT)

install:
	cp -f upsd /usr/local/sbin/
	#cp -f upsmon /etc/init.d/

uninstall:
	#rm -f /etc/init.d/upsmon
	rm -f /usr/local/sbin/upsd

