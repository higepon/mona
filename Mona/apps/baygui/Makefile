all:
	@cd gbball; $(MAKE) $@
	@cd gclock; $(MAKE) $@
	@cd grunner; $(MAKE) $@
	@cd guitest1; $(MAKE) $@
	@cd gversion; $(MAKE) $@

install:
	@cd gbball; $(MAKE) $@
	@cd gclock; $(MAKE) $@
	@cd grunner; $(MAKE) $@
	@cd guitest1; $(MAKE) $@
	@cd gversion; $(MAKE) $@

clean: touchdepend
	@cd gbball; $(MAKE) $@
	@cd gclock; $(MAKE) $@
	@cd grunner; $(MAKE) $@
	@cd guitest1; $(MAKE) $@
	@cd gversion; $(MAKE) $@

depend: touchdepend
	@cd gbball; $(MAKE) $@
	@cd gclock; $(MAKE) $@
	@cd grunner; $(MAKE) $@
	@cd guitest1; $(MAKE) $@
	@cd gversion; $(MAKE) $@

distclean:
	rm -f gbball/dependencies
	rm -f gclock/dependencies
	rm -f grunner/dependencies
	rm -f guitest1/dependencies
	rm -f gversion/dependencies

touchdepend:
	touch gbball/dependencies
	touch gclock/dependencies
	touch grunner/dependencies
	touch guitest1/dependencies
	touch gversion/dependencies
