all:
	@cd key; $(MAKE) $@
	@cd mouse; $(MAKE) $@
#	@cd shell; $(MAKE) $@
	@cd shell; $(MAKE) -f Makefile.old $@
	@cd gui; $(MAKE) $@
	@cd 1linesh; $(MAKE) $@
	@cd file; $(MAKE) $@
	@cd elf; $(MAKE) $@
	@cd moniter; $(MAKE) $@

install:
	@cd key; $(MAKE) $@
	@cd mouse; $(MAKE) $@
#	@cd shell; $(MAKE) $@
	@cd shell; $(MAKE) -f Makefile.old $@
	@cd gui; $(MAKE) $@
	@cd 1linesh; $(MAKE) $@
	@cd file; $(MAKE) $@
	@cd elf; $(MAKE) $@
	@cd moniter; $(MAKE) $@

clean: touchdepend
	@cd key; $(MAKE) $@
	@cd mouse; $(MAKE) $@
#	@cd shell; $(MAKE) $@
	@cd shell; $(MAKE) -f Makefile.old $@
	@cd gui; $(MAKE) $@
	@cd 1linesh; $(MAKE) $@
	@cd file; $(MAKE) $@
	@cd elf; $(MAKE) $@
	@cd moniter; $(MAKE) $@

depend: touchdepend
	@cd key; $(MAKE) $@
	@cd mouse; $(MAKE) $@
	@cd shell; $(MAKE) -f Makefile.old $@
	@cd gui; $(MAKE) $@
	@cd 1linesh; $(MAKE) $@
	@cd file; $(MAKE) $@
	@cd elf; $(MAKE) $@
	@cd moniter; $(MAKE) $@

distclean:
	rm -f key/dependencies
	rm -f mouse/dependencies
	rm -f shell/dependencies
	rm -f gui/dependencies
	rm -f 1linesh/dependencies
	rm -f file/dependencies
	rm -f elf/dependencies
	rm -f moniter/dependencies

touchdepend:
	touch key/dependencies
	touch mouse/dependencies
	touch shell/dependencies
	touch gui/dependencies
	touch 1linesh/dependencies
	touch file/dependencies
	touch elf/dependencies
	touch moniter/dependencies
