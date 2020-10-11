sbin_PROGRAMS = gm_config
gm_config_SOURCES = gm_config.cc gmconfig.cc
gm_config_LDADD = -L../../shared -lgmshared -L../../queue -lgmq
gm_config_CPPFLAGS = -I../../shared  -I../../queue

AM_CXXFLAGS = \
 -DLOG_FILES=\"$(localstatedir)/log/gmonitor\" \
 -DMAX_SERVERS=$(gmon_max_servers) \
 -DMAX_SERVICES=$(gmon_max_services) \
 -Wall $(gmon_debug_global)

