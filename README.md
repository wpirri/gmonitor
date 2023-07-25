Gnu-Monitor - gnumonitor@gnumonitor.com.ar

GNU-Monitor is a transactional monitor that allows client/server applications
to be developed with minimal effort. It consists of transaction routing modules
that ensure data integrity and recovery on abnormal termination.

Modulos que lo componen

gmon_config

	Interface interactiva de administracion del sistema GNU-Monitor.
  
gmond

	Levanta el demonio de conexiones gmd y los programas servidores gmq
	que están configurados en la base de datos. Durante la instalación se
	hace un link a este script desde /etc/init.d o /etc/rc.d. Este script
	acepta los parámetros start, stop, status, restart y reload.
  
gmd

	Administrador de conexiones, recibe los mensajes a travez de TCP/IP,
	inetd o xinetd y llos envía a a cola del ruteador. Cuando se utiliza
	sin inetd o xinetd escucha en el puerto 5533.
  
gmt

	Monitor Transaccional, se encarga de rutear los mensajes al servidor
	correspondiente y llevar el control de las transacciones que realice
	el cliente en esa conexión.
  
gmq

	Este proceso se levanta uno para cada servidor pasandole como parametro
	el nombre del server que debe administrar.
	Arma una cola de mensajes y actualiza la base de datos para que el
	monitor (gmt) pueda rutear los mensajes que le corresponden al
	servidor.
  
libgmq

	Libreria para linkear con servers stand-alone.
  
libgmqw

	Libreria para linkear con servers waited.
  
gm_config

	Servidor de configuracion dinamica de servicios.
  
gm_default

	Servidor de ejemplo, resuelve la función ".eco"
  
gm_timer

	Servidor de timers.
  
gm_transac

	Servidor de administracion de transacciones.
  
libgmc

	Librería del cliente, debe ser linkeada con el cliente que utilice el
	monitor.

Para mayor información acerca del sistema debe buscarse en los manuales que se
distribuyen junto con los fuentes.

Instalación desde los fuentes:
-------------------------------------------------------------------------------
  Dentro del directorio system ejecutar make y luego sudo make install

