#! /bin/sh

case "$1" in
install)
	# xinetd
	if [ -d /etc/xinetd.d ] && [ ! -f /etc/xinetd.d/gmonitor ]
	then
		cp gmonitor /etc/xinetd.d/
		echo "	server          = ${2}/gmd" 	>> /etc/xinetd.d/gmonitor
		echo "	server_args     = --mode=xinetd"    	>> /etc/xinetd.d/gmonitor
		echo "}" 					>> /etc/xinetd.d/gmonitor
	fi
	# services
	x=`grep "^gmonitor" /etc/services`
	if [ "X${x}" = "X" ]
	then
		echo "gmonitor        5533/tcp        # Gnu-Monitor" >> /etc/services
	fi

	# inetd
	#if [ -f /etc/inetd.conf ]
	#then
	#	x=`grep "^gmonitor" /etc/inetd.conf`
	#	if [ "X${x}" = "X" ]
	#	then
	#		echo "gmonitor  stream  tcp  nowait  root  ${3}/gmd --mode=inetd" >> 	/etc/inetd.conf
	#	fi
	#fi
	;;
uninstall)
	rm -f /etc/xinetd.d/gmonitor
	;;
esac

