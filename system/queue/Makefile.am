sbin_PROGRAMS = gmq
lib_LTLIBRARIES = libgmq.la libgmqw.la

gmq_SOURCES = gmsbase.cc gmq.cc gms.cc
gmq_LDADD = -L../shared -lgmshared
gmq_CPPFLAGS = -I../shared

libgmq_la_SOURCES = gmsbase.cc gmq.cc
libgmq_la_LIBADD = -L../shared -lgmshared
libgmq_la_CPPFLAGS = -I../shared

libgmqw_la_SOURCES = gmsbase.cc gmswaited.cc
libgmqw_la_LIBADD = -L../shared -lgmshared
libgmqw_la_CPPFLAGS = -I../shared

pkginclude_HEADERS =   \
 gms.h                 \
 gmswaited.h           \
 gmsbase.h   \
 ../shared/gmshared.h  \
 ../shared/gmidat.h    \
 ../shared/gmbuffer.h  \
 ../shared/gmconst.h   \
 ../shared/gmontdb.h   \
 ../shared/sincmem.h   \
 ../shared/sincro.h    \
 ../shared/shmem.h     \
 ../shared/cmsg.h      \
 ../shared/gmisc.h     \
 ../shared/gmerror.h   \
 ../shared/gmessage.h  \
 ../shared/glog.h      \
 ../shared/msgtype.h   \
 ../shared/svcstru.h   \
 ../shared/gmheader.h

AM_CXXFLAGS = \
 -DBIN_MONITOR=\"$(sbindir)/gmt\" \
 -DMONITOR_CONFIG_PATH=\"$(gmon_tbldir)\" \
 -DLOG_FILES=\"$(gmon_logdir)\" \
 -DMAX_SERVERS=$(gmon_max_servers) \
 -DMAX_SERVICES=$(gmon_max_services) \
 -Wall $(gmon_debug_global)

install-data-local:
	$(mkinstalldirs) $(gmon_logdir)
