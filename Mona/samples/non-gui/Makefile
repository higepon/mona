all:
	@cd hello && $(MAKE) $@
	@cd infones && $(MAKE) $@
#	@cd kanji && $(MAKE) $@
	@cd yamami && $(MAKE) $@

install:
	@cd hello && $(MAKE) $@
	@cd infones && $(MAKE) $@
#	@cd kanji && $(MAKE) $@
	@cd yamami && $(MAKE) $@

clean: touchdepend
	@cd hello && $(MAKE) $@
	@cd infones && $(MAKE) $@
#	@cd kanji && $(MAKE) $@
	@cd yamami && $(MAKE) $@

depend: touchdepend
	@cd hello && $(MAKE) $@
	@cd infones && $(MAKE) $@
#	@cd kanji && $(MAKE) $@
	@cd yamami && $(MAKE) $@

distclean:
	rm -f hello/dependencies
	rm -f infones/dependencies
#	rm -f kanji/dependencies
	rm -f yamami/dependencies

touchdepend:
	touch hello/dependencies
	touch infones/dependencies
#	touch kanji/dependencies
	touch yamami/dependencies
