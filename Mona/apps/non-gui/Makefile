all:
	@cd noiz2bg; $(MAKE) $@
	@cd reversi; $(MAKE) $@
	@cd jpegdemo; $(MAKE) $@
#	@cd clock; $(MAKE) $@
	@cd nic; $(MAKE) $@
	@cd waba; $(MAKE) $@

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
	touchdepend noiz2bg/dependencies
	touchdepend reversi/dependencies
	touchdepend jpegdemo/dependencies
#	touchdepend clock/dependencies
	touchdepend waba/dependencies
