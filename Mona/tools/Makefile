all:
	@cd fat_write; $(MAKE) $@
	@cd bit2mnf; $(MAKE) $@
	@cd monaelf; $(MAKE) $@

clean: touchdepend
	@cd fat_write; $(MAKE) $@
	@cd bit2mnf; $(MAKE) $@
	@cd monaelf; $(MAKE) $@

distclean:
	rm -f fat_write/dependencies

install:
	@cd bit2mnf; $(MAKE) $@

depend: touchdepend
	@cd fat_write; $(MAKE) $@

touchdepend:
	touch fat_write/dependencies
