#!/bin/sh

OWNER="$1"

if [ "x${OWNER}" = "x" ]; then
	exit
fi

ipcs -m | grep $OWNER | awk '{ print $2 }' | while read ID; do
	echo "Eliminando memoria compartida ${ID}"
	ipcrm shm $ID
done

ipcs -q | grep $OWNER | awk '{ print $2 }' | while read ID; do
	echo "Eliminando cola ${ID}"
	ipcrm msg $ID
done

ipcs -s | grep $OWNER | awk '{ print $2 }' | while read ID; do
	echo "Eliminando semaforo ${ID}"
	ipcrm sem $ID
done

ps -eaf | grep ^$OWNER | awk '{ print $2 }' | while read PID; do
	echo "Eliminando proceso ${PID}"
	kill $PID
	kill -9 $PID >/dev/null 2>&1
done
