all:
	@cd key; $(MAKE) $@
	@cd mouse; $(MAKE) $@
	@cd shell; $(MAKE) $@
	@cd gui; $(MAKE) $@
	@cd 1linesh; $(MAKE) $@
	@cd file; $(MAKE) $@

install:
	@cd key; $(MAKE) $@
	@cd mouse; $(MAKE) $@
	@cd shell; $(MAKE) $@
	@cd gui; $(MAKE) $@
	@cd 1linesh; $(MAKE) $@
	@cd file; $(MAKE) $@

clean: touchdepend
	@cd key; $(MAKE) $@
	@cd mouse; $(MAKE) $@
	@cd shell; $(MAKE) $@
	@cd gui; $(MAKE) $@
	@cd 1linesh; $(MAKE) $@
	@cd file; $(MAKE) $@

depend: touchdepend
	@cd key; $(MAKE) $@
	@cd mouse; $(MAKE) $@
	@cd shell; $(MAKE) $@
	@cd gui; $(MAKE) $@
	@cd 1linesh; $(MAKE) $@
	@cd file; $(MAKE) $@

distclean:
	rm -f key/dependencies
	rm -f mouse/dependencies
	rm -f shell/dependencies
	rm -f gui/dependencies
	rm -f 1linesh/dependencies
	rm -f file/dependencies

touchdepend:
	touch key/dependencies
	touch mouse/dependencies
	touch shell/dependencies
	touch gui/dependencies
	touch 1linesh/dependencies
	touch file/dependencies
