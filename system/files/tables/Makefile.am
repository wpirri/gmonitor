install-data-local:
	$(mkinstalldirs) $(gmon_tbldir)
	if test ! -f $(gmon_tbldir)/funcion_parametro.tab ; then $(install_sh_DATA) funcion_parametro.tab $(gmon_tbldir) ; fi
	if test ! -f $(gmon_tbldir)/funcion.tab ; then $(install_sh_DATA) funcion.tab $(gmon_tbldir) ; fi
	if test ! -f $(gmon_tbldir)/server_parametro.tab ; then $(install_sh_DATA) server_parametro.tab $(gmon_tbldir) ; fi
	if test ! -f $(gmon_tbldir)/server.tab ; then $(install_sh_DATA) server.tab $(gmon_tbldir); fi
uninstall-local:
	rm -rf $(gmon_tbldir)

