all:
	@cd fat_write; $(MAKE) $@
	@cd bit2mnf; $(MAKE) $@
	@cd monaelf; $(MAKE) $@

install:
	@cd bit2mnf; $(MAKE) $@

clean: touchdepend
	@cd fat_write; $(MAKE) $@
	@cd bit2mnf; $(MAKE) $@
	@cd monaelf; $(MAKE) $@

depend: touchdepend
	@cd fat_write; $(MAKE) $@

distclean:
	rm -f fat_write/dependencies

touchdepend:
	touch fat_write/dependencies
