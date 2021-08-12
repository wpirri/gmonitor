#!/bin/sh

if [ -f /etc/gmonitor ]; then
  . /etc/gmonitor
else
  if [ -f /etc/sysconfig/gmonitor ]; then
    . /etc/sysconfig/gmonitor
  else
    if [ -f /usr/etc/gmonitor ]; then
      . /usr/etc/gmonitor
    else
      if [ -f /usr/etc/sysconfig/gmonitor ]; then
        . /usr/etc/sysconfig/gmonitor
      fi
    fi
  fi
fi

stop_proccess()
{
        for p in `${PS_EAF} | grep -v grep | grep -v gmon_stop | grep "${1}" | awk '{ print $2 }'`
        do
                kill ${p} >/dev/null 2>&1
        done
}

stop_one_service()
{
        servicio=`grep "^${1}|" $SERVER_CONF/server.tab | awk -F'|' '{ print $1 }'`
        descripcion=`grep "^${1}|" $SERVER_CONF/server.tab | awk -F'|' '{ print $2 }'`
        modo=`grep "^${1}|" $SERVER_CONF/server.tab | awk -F'|' '{ print $3 }'`
        server=`grep "^${1}|" $SERVER_CONF/server.tab | awk -F'|' '{ print $4 }'`

        if [ "X${servicio}" = "X" ]
        then
                exit 1
        fi

        if [ "X${modo}" = "X0" ] || [ "X${modo}" = "X1" ]
        then
                echo "Deteniendo: ${descripcion}"
                stop_proccess "$SERVER_BIN/gmq -n $servicio"
        else
                echo "Deteniendo: ${descripcion}"
                stop_proccess "$server -n $servicio"
        fi
}

stop_all_services()
{
        service_list=""
        server_tab=`cat $SERVER_CONF/server.tab | grep -v "^#" | awk -F'|' '{ print $1 }'`
        for servicio in $server_tab
        do
                service_list="${servicio} ${service_list}"
        done

        for i in $service_list
        do
                stop_one_service $i
        done
}

stop_all_services

gmt_pid=`ps -eaf | grep -v grep | grep $SERVER_BIN/gmt | awk '{ print $2; }'`
kill ${gmt_pid}

