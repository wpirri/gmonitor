 #!/usr/bin/make -f

# =============================================================================
# instalar
#	Entorno de desarrollo mínimo: build-essential vim net-tools nfs-common mc minicom
#	Para Desarrollo: xinetd libssl-dev
#	Para Produccion: xinetd libssl
#

include configure.mk

all:
	chmod 0755 fixeol.sh
	./fixeol.sh
	make -C shared
	make -C listener
	make -C router
	make -C queue
	make -C client
	make -C servers
	make -C tools
#	make -C monitor

clean:
	make -C shared clean
	make -C listener clean
	make -C router clean
	make -C queue clean
	make -C client clean
	make -C servers clean
	make -C tools clean
	make -C monitor clean

install:
	make -C files install
	make -C shared install
	make -C listener install
	make -C router install
	make -C queue install
	make -C client install
	make -C servers install
	make -C tools install
#	make -C monitor install

uninstall:
	make -C files uninstall
	make -C shared uninstall
	make -C listener uninstall
	make -C router uninstall
	make -C queue uninstall
	make -C client uninstall
	make -C servers uninstall
	make -C tools uninstall
	make -C monitor uninstall

test:
	@echo "RUN_USER:         "$(RUN_USER)
	@echo "INST_USER:        "$(INST_USER)
	@echo "INST_LIBDIR:      "$(INST_LIBDIR)
	@echo "INST_HEADDIR:     "$(INST_HEADDIR)
	@echo "INST_BINDIR:      "$(INST_BINDIR)
	@echo "INST_SBINDIR:     "$(INST_SBINDIR)
	@echo "INST_ETCDIR:      "$(INST_ETCDIR)
	@echo "INST_VARDIR:      "$(INST_VARDIR)
	@echo "INST_INCDIR:      "$(INST_INCDIR)
	@echo "INST_LOGDIR:      "$(INST_LOGDIR)
	@echo "PROCESSOR-PARAMS: "$(PROCESSOR-PARAMS)

	$(if $(findstring arm,$(MACHINE)) ,@echo "Compilando para RaspBerry Pi", @echo "Compilando para PC" )

fixeol:
	dos2unix fixeol.sh
	./fixeol.sh
	
