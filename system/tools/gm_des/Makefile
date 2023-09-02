#!/usr/bin/make -f

.c.o:
	cc $(PROCESSOR-PARAMS) -c $?

all: gmcrypt

gmcrypt: des.o sec.o
	cc $(PROCESSOR-PARAMS) -O des.o sec.o -o gmcrypt 

clean:
	rm -f des.o sec.o gmcrypt

install:
	cp -f gmcrypt /usr/local/bin/

uninstall:
	rm -f /usr/local/bin/gmcrypt

