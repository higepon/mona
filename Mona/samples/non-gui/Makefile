all:
	@cd hello; $(MAKE) $@
#	@cd kanji; $(MAKE) $@

install:
	@cd hello; $(MAKE) $@
#	@cd kanji; $(MAKE) $@

clean: touchdepend
	@cd hello; $(MAKE) $@
#	@cd kanji; $(MAKE) $@

depend: touchdepend
	@cd hello; $(MAKE) $@
#	@cd kanji; $(MAKE) $@

distclean:
	rm -f hello/dependencies
#	rm -f kanji/dependencies

touchdepend:
	touch hello/dependencies
#	touch kanji/dependencies
