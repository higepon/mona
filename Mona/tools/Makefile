all:
	@cd fat_write; $(MAKE) $@
	@cd bit2mnf; $(MAKE) $@
	@cd monaelf; $(MAKE) $@
	@cd elf_tool; $(MAKE) $@

install:
	@cd fat_write; $(MAKE)
	@cd bit2mnf; $(MAKE) $@
	@cd monaelf; $(MAKE)
	@cd elf_tool; $(MAKE)

clean: touchdepend
	@cd fat_write; $(MAKE) $@
	@cd bit2mnf; $(MAKE) $@
	@cd monaelf; $(MAKE) $@
	@cd elf_tool; $(MAKE) $@

depend: touchdepend
	@cd fat_write; $(MAKE) $@
	@cd elf_tool; $(MAKE) $@

distclean:
	rm -f fat_write/dependencies
	rm -f elf_tool/dependencies

touchdepend:
	touch fat_write/dependencies
	touch elf_tool/dependencies
