all:
	@cd button; $(MAKE) $@
	@cd label; $(MAKE) $@
	@cd bitmap; $(MAKE) $@

install:
	@cd button; $(MAKE) $@
	@cd label; $(MAKE) $@
	@cd bitmap; $(MAKE) $@

clean: touchdepend
	@cd button; $(MAKE) $@
	@cd label; $(MAKE) $@
	@cd bitmap; $(MAKE) $@

depend: touchdepend
	@cd button; $(MAKE) $@
	@cd label; $(MAKE) $@
	@cd bitmap; $(MAKE) $@

distclean:
	rm -f button/dependencies
	rm -f label/dependencies
	rm -f bitmap/dependencies

touchdepend:
	touch button/dependencies
	touch label/dependencies
	touch bitmap/dependencies
