all:
	@cd lib && $(MAKE) $@
	@cd servers && $(MAKE) $@
	@cd kernel && $(MAKE) $@

install:
	@cd lib && $(MAKE) $@
	@cd servers && $(MAKE) $@
	@cd kernel && $(MAKE) $@

clean: touchdepend
	@cd kernel && $(MAKE) $@
	@cd lib && $(MAKE) $@
	@cd servers && $(MAKE) $@

depend: touchdepend
	@cd kernel && $(MAKE) $@
	@cd lib && $(MAKE) $@
	@cd servers && $(MAKE) $@

distclean:
	rm -f kernel/dependencies
	@cd lib && $(MAKE) $@
	@cd servers && $(MAKE) $@

touchdepend:
	touch kernel/dependencies
