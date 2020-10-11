sbin_PROGRAMS = gm_sbuffer
gm_sbuffer_SOURCES = gm_sbuffer.cc gmsbuffer.cc
gm_sbuffer_LDADD = -L../../shared -lgmshared -L../../queue -lgmq
gm_sbuffer_CPPFLAGS = -I../../shared -I../../queue

AM_CXXFLAGS = \
 -DLOG_FILES=\"$(localstatedir)/log/gmonitor\" \
 -DMAX_SERVERS=$(gmon_max_servers) \
 -DMAX_SERVICES=$(gmon_max_services) \
 -Wall $(gmon_debug_global)

