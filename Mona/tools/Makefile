all:
	@cd fat_write; $(MAKE) $@
	@cd bit2mnf; $(MAKE) $@
	@cd monaelf; $(MAKE) $@

clean:
	@cd fat_write; $(MAKE) $@
	@cd bit2mnf; $(MAKE) $@
	@cd monaelf; $(MAKE) $@

distclean:
	rm -f fat_write/dependencies

install:
	@cd bit2mnf; $(MAKE) $@

depend:
	touch fat_write/dependencies
	@cd fat_write; $(MAKE) $@
