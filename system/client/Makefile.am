lib_LTLIBRARIES = libgmc.la
libgmc_la_SOURCES = gmc.cc
libgmc_la_LIBADD = -L../shared -lgmshared
libgmc_la_CPPFLAGS = -I../shared

pkginclude_HEADERS =  \
 gmc.h                \
 ../shared/gmshared.h \
 ../shared/ctcp.h     \
 ../shared/gmidat.h   \
 ../shared/gmerror.h  \
 ../shared/gmessage.h \
 ../shared/gmheader.h \
 ../shared/svcstru.h  \
 ../shared/gmbuffer.h

AM_CXXFLAGS = \
-DGM_COMM_MSG_LEN=$(gm_comm_msg_len) \
-Wall $(gmon_debug_global)
