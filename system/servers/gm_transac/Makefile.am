sbin_PROGRAMS = gm_transac
gm_transac_SOURCES = gm_transac.cc transac.cc wtimer.cc
gm_transac_LDADD = -L../../shared -lgmshared -L../../queue -lgmqw
gm_transac_CPPFLAGS = -I../../shared -I../../queue

AM_CXXFLAGS = \
 -DLOG_FILES=\"$(localstatedir)/log/gmonitor\" \
 -DMAX_SERVERS=$(gmon_max_servers) \
 -DMAX_SERVICES=$(gmon_max_services) \
 -Wall $(gmon_debug_global)

