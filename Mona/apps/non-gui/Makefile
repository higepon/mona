all:
	@cd noiz2bg; $(MAKE) $@
	@cd reversi; $(MAKE) $@
	@cd jpegdemo; $(MAKE) $@
#	@cd clock; $(MAKE) $@
	@cd nic; $(MAKE) $@
	@cd waba; $(MAKE) $@
	@cd tube; $(MAKE) $@

install:
	@cd noiz2bg; $(MAKE) $@
	@cd reversi; $(MAKE) $@
	@cd jpegdemo; $(MAKE) $@
#	@cd clock; $(MAKE) $@
	@cd nic; $(MAKE) $@
	@cd waba; $(MAKE) $@
	@cd tube; $(MAKE) $@

clean: touchdepend
	@cd noiz2bg; $(MAKE) $@
	@cd reversi; $(MAKE) $@
	@cd jpegdemo; $(MAKE) $@
#	@cd clock; $(MAKE) $@
	@cd nic; $(MAKE) $@
	@cd waba; $(MAKE) $@
	@cd tube; $(MAKE) $@

depend: touchdepend
	@cd noiz2bg; $(MAKE) $@
	@cd reversi; $(MAKE) $@
	@cd jpegdemo; $(MAKE) $@
#	@cd clock; $(MAKE) $@
	@cd nic; $(MAKE) $@
	@cd waba; $(MAKE) $@

distclean:
	rm -f noiz2bg/dependencies
	rm -f reversi/dependencies
	rm -f jpegdemo/dependencies
#	rm -f clock/dependencies
	rm -f waba/dependencies
	@cd nic; $(MAKE) $@

touchdepend:
	touch noiz2bg/dependencies
	touch reversi/dependencies
	touch jpegdemo/dependencies
#	touch clock/dependencies
	touch waba/dependencies
