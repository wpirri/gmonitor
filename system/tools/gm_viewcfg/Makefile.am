bin_PROGRAMS = gm_viewcfg

gm_viewcfg_SOURCES = gm_viewcfg.cc
gm_viewcfg_LDADD = -L../../shared -lgmshared -L../../client -lgmc -lssl -lcrypto
gm_viewcfg_CPPFLAGS = -I../../shared -I../../client

AM_CXXFLAGS = \
 -DBIN_MONITOR=\"$(sbindir)/gmt\" \
 -DMONITOR_CONFIG_PATH=\"$(gmon_tbldir)\" \
 -DLOG_FILES=\"$(gmon_logdir)\" \
 -DMAX_SERVERS=$(gmon_max_servers) \
 -DMAX_SERVICES=$(gmon_max_services) \
 -Wall $(gmon_debug_global)
