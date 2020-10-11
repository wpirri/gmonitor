sbin_PROGRAMS = gm_timer
gm_timer_SOURCES = gm_timer.cc gmtimer.cc wtimer.cc
gm_timer_LDADD = -L../../shared -lgmshared -L../../queue -lgmqw
gm_timer_CPPFLAGS = -I../../shared -I../../queue

AM_CXXFLAGS = \
 -DLOG_FILES=\"$(localstatedir)/log/gmonitor\" \
 -DMAX_SERVERS=$(gmon_max_servers) \
 -DMAX_SERVICES=$(gmon_max_services) \
 -Wall $(gmon_debug_global)

