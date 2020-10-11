SUBDIRS = gm_config gm_default gm_timer gm_transac gm_sbuffer gm_squeue

install-data-local:
	./update-tables.sh gm_config $(gmon_tbldir) server $(sbindir)/gm_config
	./update-tables.sh gm_config $(gmon_tbldir) funcion

	./update-tables.sh gm_default $(gmon_tbldir) server $(sbindir)/gm_default
	./update-tables.sh gm_default $(gmon_tbldir) funcion

	./update-tables.sh gm_timer $(gmon_tbldir) server $(sbindir)/gm_timer
	./update-tables.sh gm_timer $(gmon_tbldir) funcion

	./update-tables.sh gm_transac $(gmon_tbldir) server $(sbindir)/gm_transac
	./update-tables.sh gm_transac $(gmon_tbldir) funcion

	./update-tables.sh gm_sbuffer $(gmon_tbldir) server $(sbindir)/gm_sbuffer
	./update-tables.sh gm_sbuffer $(gmon_tbldir) funcion

	./update-tables.sh gm_squeue $(gmon_tbldir) server $(sbindir)/gm_squeue
	./update-tables.sh gm_squeue $(gmon_tbldir) funcion
