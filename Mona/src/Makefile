all:
	@cd kernel; $(MAKE) $@
	@cd lib; $(MAKE) $@
	@cd servers; $(MAKE) $@

install:
	@cd kernel; $(MAKE) $@
	@cd lib; $(MAKE) $@
	@cd servers; $(MAKE) $@

clean: touchdepend
	@cd kernel; $(MAKE) $@
	@cd lib; $(MAKE) $@
	@cd servers; $(MAKE) $@

distclean:
	rm -f kernel/dependencies
	@cd lib; $(MAKE) $@
	@cd servers; $(MAKE) $@

depend:
	@cd kernel; $(MAKE) $@
	@cd lib; $(MAKE) $@
	@cd servers; $(MAKE) $@

touchdepend:
	touch kernel/dependencies
