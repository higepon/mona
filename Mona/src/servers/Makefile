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
	@cd process; $(MAKE) $@
	@cd pe; $(MAKE) $@
#	@cd mones; $(MAKE) $@
	@cd baygui; $(MAKE) $@

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
	@cd process; $(MAKE) $@
	@cd pe; $(MAKE) $@
#	@cd mones; $(MAKE) $@
	@cd baygui; $(MAKE) $@

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
	@cd process; $(MAKE) $@
	@cd pe; $(MAKE) $@
#	@cd mones; $(MAKE) $@
	@cd baygui; $(MAKE) $@

depend: touchdepend
	@cd key; $(MAKE) $@
	@cd mouse; $(MAKE) $@
	@cd shell; $(MAKE) -f Makefile.old $@
	@cd gui; $(MAKE) $@
	@cd 1linesh; $(MAKE) $@
	@cd file; $(MAKE) $@
	@cd elf; $(MAKE) $@
	@cd moniter; $(MAKE) $@
	@cd process; $(MAKE) $@
	@cd pe; $(MAKE) $@
#	@cd mones; $(MAKE) $@
	@cd baygui; $(MAKE) $@

distclean:
	rm -f key/dependencies
	rm -f mouse/dependencies
	rm -f shell/dependencies
	rm -f gui/dependencies
	rm -f 1linesh/dependencies
	rm -f file/dependencies
	rm -f elf/dependencies
	rm -f moniter/dependencies
	rm -f process/dependencies
	rm -f pe/dependencies
#	rm -f mones/dependencies
	rm -f baygui/dependencies
	
touchdepend:
	touch key/dependencies
	touch mouse/dependencies
	touch shell/dependencies
	touch gui/dependencies
	touch 1linesh/dependencies
	touch file/dependencies
	touch elf/dependencies
	touch moniter/dependencies
	touch process/dependencies
	touch pe/dependencies
#	touch mones/dependencies
	touch baygui/dependencies
