sbin_PROGRAMS = gmt
gmt_SOURCES = gmt.cc
gmt_LDADD = -L../shared -lgmshared -lssl -lcrypto
gmt_CPPFLAGS = -I../shared

AM_CXXFLAGS = \
 -DBIN_MONITOR=\"$(sbindir)/gmt\" \
 -DMONITOR_CONFIG_PATH=\"$(gmon_tbldir)\" \
 -DLOG_FILES=\"$(gmon_logdir)\" \
 -DMAX_SERVERS=$(gmon_max_servers) \
 -DMAX_SERVICES=$(gmon_max_services) \
 -Wall $(gmon_debug_global)

install-data-local:
	groupadd -f gmonitor
	useradd -c "Gnu Monitor" -d $(gmon_tbldir) -g gmonitor -s /bin/false gmonitor || echo "Ya existe el usuario"
	$(mkinstalldirs) $(gmon_logdir)
	chown gmonitor:gmonitor $(gmon_logdir)

