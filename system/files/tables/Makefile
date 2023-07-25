#!/usr/bin/make -f

include ../../configure.mk

install:
	$(MKDIR)  $(INST_VARDIR)
	if test ! -f $(INST_VARDIR)/funcion_parametro.tab ; then $(CP) funcion_parametro.tab $(INST_VARDIR)/ ; fi
	if test ! -f $(INST_VARDIR)/funcion.tab ; then $(CP) funcion.tab $(INST_VARDIR)/ ; fi
	if test ! -f $(INST_VARDIR)/server_parametro.tab ; then $(CP) server_parametro.tab $(INST_VARDIR)/ ; fi
	if test ! -f $(INST_VARDIR)/server.tab ; then $(CP) server.tab $(INST_VARDIR)/ ; fi

uninstall:
	$(RM) $(INST_VARDIR)/funcion_parametro.tab
	$(RM) $(INST_VARDIR)/funcion.tab
	$(RM) $(INST_VARDIR)/server_parametro.tab
	$(RM) $(INST_VARDIR)/server.tab
