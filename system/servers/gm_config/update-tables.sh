#!/bin/sh
cat conf/$2.tab | while read line
do
	if [ "X${2}" = "Xserver" ]
	then
		grep "$line" $1/$2.tab >/dev/null 2>&1 || echo "$line$3" >> $1/$2.tab
	else
		grep "$line" $1/$2.tab >/dev/null 2>&1 || echo "$line" >> $1/$2.tab
	fi
done
