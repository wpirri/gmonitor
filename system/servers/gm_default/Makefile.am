sbin_PROGRAMS = gm_default
gm_default_SOURCES = gm_default.cc
gm_default_LDADD = -L../../shared -lgmshared -L../../queue -lgmq
gm_default_CPPFLAGS = -I../../shared -I../../queue

AM_CXXFLAGS = \
 -DLOG_FILES=\"$(localstatedir)/log/gmonitor\" \
 -DMAX_SERVERS=$(gmon_max_servers) \
 -DMAX_SERVICES=$(gmon_max_services) \
 -Wall $(gmon_debug_global)

