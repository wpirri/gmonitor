sbin_PROGRAMS = gm_squeue
gm_squeue_SOURCES = gmsafile.cc gmsaf.cc gm_squeue.cc
gm_squeue_LDADD = -L../../shared -lgmshared -L../../queue -lgmq
gm_squeue_CPPFLAGS = -I../../shared -I../../queue

AM_CXXFLAGS = \
 -DSAF_FILES=\"$(localstatedir)/gmonitor/saf\" \
 -DLOG_FILES=\"$(localstatedir)/log/gmonitor\" \
 -DMAX_SERVERS=$(gmon_max_servers) \
 -DMAX_SERVICES=$(gmon_max_services) \
 -Wall $(gmon_debug_global)

install-data-local:
	$(mkinstalldirs) $(localstatedir)/gmonitor/saf
