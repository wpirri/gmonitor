bin_PROGRAMS = gm_loglevel

gm_loglevel_SOURCES = gm_loglevel.cc classapp.cc
gm_loglevel_LDADD = -L../../client -lgmc -L../../shared -lgmshared
gm_loglevel_CPPFLAGS = -I../../shared -I../../client

AM_CXXFLAGS = \
 -Wall $(gmon_debug_global)
