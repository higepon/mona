all:
	@cd reversi; $(MAKE) $@
	@cd ps; $(MAKE) $@
	@cd 2chAA; $(MAKE) $@
	@cd noiz2bg; $(MAKE) $@
	@cd fire; $(MAKE) $@
	@cd launcher; $(MAKE) $@
	@cd waba; $(MAKE) $@
	@cd shell; $(MAKE) $@
	@cd clock; $(MAKE) $@

install:
	@cd reversi; $(MAKE) $@
	@cd ps; $(MAKE) $@
	@cd 2chAA; $(MAKE) $@
	@cd noiz2bg; $(MAKE) $@
	@cd fire; $(MAKE) $@
	@cd launcher; $(MAKE) $@
	@cd waba; $(MAKE) $@
	@cd shell; $(MAKE) $@
	@cd clock; $(MAKE) $@

clean: touchdepend
	@cd reversi; $(MAKE) $@
	@cd ps; $(MAKE) $@
	@cd 2chAA; $(MAKE) $@
	@cd noiz2bg; $(MAKE) $@
	@cd fire; $(MAKE) $@
	@cd launcher; $(MAKE) $@
	@cd waba; $(MAKE) $@
	@cd shell; $(MAKE) $@
	@cd clock; $(MAKE) $@

depend: touchdepend
	@cd reversi; $(MAKE) $@
	@cd ps; $(MAKE) $@
	@cd 2chAA; $(MAKE) $@
	@cd noiz2bg; $(MAKE) $@
	@cd fire; $(MAKE) $@
	@cd launcher; $(MAKE) $@
	@cd waba; $(MAKE) $@
	@cd shell; $(MAKE) $@
	@cd clock; $(MAKE) $@

distclean:
	rm -f reversi/dependencies
	rm -f ps/dependencies
	rm -f 2chAA/dependencies
	rm -f noiz2bg/dependencies
	rm -f fire/dependencies
	rm -f launcher/dependencies
	rm -f waba/dependencies
	rm -f shell/dependencies
	rm -f clock/dependencies
#	@cd clock; $(MAKE) $@

touchdepend:
	touch reversi/dependencies
	touch ps/dependencies
	touch 2chAA/dependencies
	touch noiz2bg/dependencies
	touch fire/dependencies
	touch launcher/dependencies
	touch waba/dependencies
	touch shell/dependencies
	touch clock/dependencies
#	@cd clock; $(MAKE) $@
