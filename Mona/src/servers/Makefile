all:
	@cd key; $(MAKE) $@
	@cd mouse; $(MAKE) $@
	@cd shell; $(MAKE) $@
	@cd gui; $(MAKE) $@
	@cd 1linesh; $(MAKE) $@

install:
	@cd key; $(MAKE) $@
	@cd mouse; $(MAKE) $@
	@cd shell; $(MAKE) $@
	@cd gui; $(MAKE) $@
	@cd 1linesh; $(MAKE) $@

clean: touchdepend
	@cd key; $(MAKE) $@
	@cd mouse; $(MAKE) $@
	@cd shell; $(MAKE) $@
	@cd gui; $(MAKE) $@
	@cd 1linesh; $(MAKE) $@

distclean:
	rm -f key/dependencies
	rm -f mouse/dependencies
	rm -f shell/dependencies
	rm -f gui/dependencies
	rm -f 1linesh/dependencies

depend: touchdepend
	@cd key; $(MAKE) $@
	@cd mouse; $(MAKE) $@
	@cd shell; $(MAKE) $@
	@cd gui; $(MAKE) $@
	@cd 1linesh; $(MAKE) $@

touchdepend:
	touch key/dependencies
	touch mouse/dependencies
	touch shell/dependencies
	touch gui/dependencies
	touch 1linesh/dependencies
