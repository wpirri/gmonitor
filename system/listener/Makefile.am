sbin_PROGRAMS = gmd
gmd_SOURCES = gmd.cc
gmd_LDADD = -L../shared -lgmshared
gmd_CPPFLAGS = -I../shared

AM_CXXFLAGS = \
-DBIN_MONITOR=\"$(sbindir)/gmt\" \
-DGM_COMM_MSG_LEN=$(gm_comm_msg_len) \
-Wall $(gmon_debug_global)

install-data-local:
	$(mkinstalldirs) $(gmon_logdir)
	./install-listener.sh install "$(install_sh_DATA)" "$(sbindir)"
uninstall-local:
	./install-listener.sh uninstall
