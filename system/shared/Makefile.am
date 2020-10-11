lib_LTLIBRARIES = libgmshared.la
libgmshared_la_SOURCES =  glog.cc \
                          ctcp.cc \
                          gmbuffer.cc \
                          gmstring.cc \
                          gmisc.cc \
                          gmerror.cc \
                          gmidat.cc \
                          gmheader.cc \
                          gmessage.cc \
                          gmcomm.cc \
                          cmsg.cc \
                          shmem.cc \
                          sincro.cc \
                          sincmem.cc \
                          gmontdb.cc

AM_CXXFLAGS = \
 -DBIN_MONITOR=\"$(sbindir)/gmt\" \
 -DMAX_SERVERS=$(gmon_max_servers) \
 -DMAX_SERVICES=$(gmon_max_services) \
 -DGM_CONFIG_KEY=$(shmemkey) \
 -Wall $(gmon_debug_global)
