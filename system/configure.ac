AC_PREREQ(2.57)
AC_INIT([gmonitor], [system-0.0.13], [gnumonitor@gnumonitor.com.ar])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([listener/gmd.cc])
AC_CONFIG_SRCDIR([client/gmc.cc])
AC_CONFIG_SRCDIR([queue/gmq.cc])
AC_CONFIG_SRCDIR([router/gmt.cc])
AC_CONFIG_SRCDIR([servers/gm_config/gm_config.cc])
AC_CONFIG_SRCDIR([servers/gm_default/gm_default.cc])
AC_CONFIG_SRCDIR([servers/gm_transac/gm_transac.cc])
AC_CONFIG_SRCDIR([shared/gmerror.h])
AM_INIT_AUTOMAKE
AC_PROG_LIBTOOL
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_MAKE_SET

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([arpa/inet.h fcntl.h netdb.h netinet/in.h stdlib.h string.h sys/socket.h sys/time.h syslog.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_STRUCT_TM

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_TYPE_SIGNAL
AC_FUNC_STAT
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([alarm dup2 gethostbyname inet_ntoa memset socket strerror strstr])

# Contol si esta definida la estructura semun
AC_MSG_CHECKING([for union semun in <sys/sem.h>])
AC_TRY_COMPILE([
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
],[union semun test_semun],[AC_MSG_RESULT(yes);
AC_DEFINE(HAVE_UNION_SEMUN,1,[Define if union semun is available.])],
AC_MSG_RESULT(no))

# Defino el directorio para las tablas internas, las colas van dentro de este
AC_SUBST(gmon_tbldir, "$localstatedir/lib/gmonitor")
# Defino el directorio para los logs del sistema
AC_SUBST(gmon_logdir, "$localstatedir/log/gmonitor")
# Defino donde van a ir los scripts de administracion y configuracion
AC_SUBST(gmon_configdir, "$sbindir")
# Defino donde va a ir el archivo de configuracion (/etc/sysconfig/gmonitor)
#AC_SUBST(gmon_sysconfigdir, "$sysconfdir")
#
# Definiciones para dimensionar estructuras de datos en memoria compartida
#
AC_SUBST(gmon_max_servers, "32")
AC_SUBST(gmon_max_services, "256")
#
# Activa la inclusion de informacion de debug en los binarios y libs
#AC_SUBST(gmon_debug_global, "-ggdb")
# Ademas de la informacion de debug incluye mensajes de ejecucion
#AC_SUBST(gmon_debug_global, "-ggdb -DDEBUG")
# Clave para la creación de memoria compartida
AC_SUBST(shmemkey, "0x131313")
# Tamaño maximo de mensaje TCP
AC_SUBST(gm_comm_msg_len, "10240")

AC_OUTPUT( Makefile \
           shared/Makefile \
           client/Makefile \
           listener/Makefile \
           queue/Makefile \
           router/Makefile \
           files/Makefile \
           files/script/Makefile \
           files/sysconfig/Makefile \
           files/tables/Makefile \
           servers/Makefile \
           servers/gm_config/Makefile \
           servers/gm_default/Makefile \
           servers/gm_timer/Makefile \
           servers/gm_transac/Makefile \
           servers/gm_sbuffer/Makefile \
           servers/gm_squeue/Makefile \
           tools/Makefile \
           tools/gm_viewcfg/Makefile \
           tools/gm_safcfg/Makefile \
           tools/gm_loglevel/Makefile )
