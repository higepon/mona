all:
	@cd gbball; $(MAKE) $@
	@cd gbrowser; $(MAKE) $@
	@cd gclock; $(MAKE) $@
	@cd ghebtn; $(MAKE) $@
	@cd grunner; $(MAKE) $@
	@cd guitest1; $(MAKE) $@
	@cd gversion; $(MAKE) $@

install:
	@cd gbball; $(MAKE) $@
	@cd gbrowser; $(MAKE) $@
	@cd gclock; $(MAKE) $@
	@cd ghebtn; $(MAKE) $@
	@cd grunner; $(MAKE) $@
	@cd guitest1; $(MAKE) $@
	@cd gversion; $(MAKE) $@

clean: touchdepend
	@cd gbball; $(MAKE) $@
	@cd gbrowser; $(MAKE) $@
	@cd gclock; $(MAKE) $@
	@cd ghebtn; $(MAKE) $@
	@cd grunner; $(MAKE) $@
	@cd guitest1; $(MAKE) $@
	@cd gversion; $(MAKE) $@

depend: touchdepend
	@cd gbball; $(MAKE) $@
	@cd gbrowser; $(MAKE) $@
	@cd gclock; $(MAKE) $@
	@cd ghebtn; $(MAKE) $@
	@cd grunner; $(MAKE) $@
	@cd guitest1; $(MAKE) $@
	@cd gversion; $(MAKE) $@

distclean:
	rm -f gbball/dependencies
	rm -f gbrowser/dependencies
	rm -f gclock/dependencies
	rm -f ghebtn/dependencies
	rm -f grunner/dependencies
	rm -f guitest1/dependencies
	rm -f gversion/dependencies

touchdepend:
	touch gbball/dependencies
	touch gbrowser/dependencies
	touch gclock/dependencies
	touch ghebtn/dependencies
	touch grunner/dependencies
	touch guitest1/dependencies
	touch gversion/dependencies
