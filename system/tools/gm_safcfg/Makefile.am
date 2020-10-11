bin_PROGRAMS = gm_safcfg

gm_safcfg_SOURCES = gm_safcfg.cc
gm_safcfg_LDADD = -L../../client -lgmc -L../../shared -lgmshared
gm_safcfg_CPPFLAGS = -I../../shared -I../../client

AM_CXXFLAGS = \
 -Wall $(gmon_debug_global)
