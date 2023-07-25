#! /bin/sh

if [ -f /etc/gmonitor ]; then
  . /etc/gmonitor
else
  if [ -f /etc/gmonitor/gmonitor ]; then
    . /etc/gmonitor/gmonitor
  else
    if [ -f /usr/etc/gmonitor ]; then
      . /usr/etc/gmonitor
    else
      if [ -f /usr/etc/gmonitor/gmonitor ]; then
        . /usr/etc/gmonitor/gmonitor
      fi
    fi
  fi
fi

get_params()
{
	grep "^${1}" $SERVER_CONF/server_parametro.tab | sed 's/|/ /g' | while true; do
		if read server_name param_name param_val; then
			param="${param} ${param_name} ${param_val}"
		else
			echo $param
		return 0
		fi
	done
}

echo "Arrancando: Router de transacciones..."
${SERVER_BIN}/gmt -d --debug $LOG_LEVEL

sleep 3

cat $SERVER_CONF/server.tab | grep -v "^#" | while read line
do
	if [ "X"`echo $line | sed 's/\ //g' ` != "X" ]; then
		servicio=`echo $line | awk -F'|' '{ print $1 }'`
		descripcion=`echo $line | awk -F'|' '{ print $2 }'`
		modo=`echo $line | awk -F'|' '{ print $3 }'`
		server=`echo $line | awk -F'|' '{ print $4 }'`
		params=`get_params $servicio`

		if [ "X${modo}" = "X0" ] || [ "X${modo}" = "X1" ]; then
			echo "Arrancando: ${descripcion}"
			${SERVER_BIN}/gmq -n ${servicio} ${params}&
		else
			echo "Arrancando: ${descripcion}"
			${server} -n ${servicio} ${params}&
		fi
		sleep 1
	fi
done

