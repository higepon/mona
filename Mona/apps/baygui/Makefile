all:
	@cd gbball && $(MAKE) $@
	@cd gclock && $(MAKE) $@
	@cd ghebtn && $(MAKE) $@
	@cd glaunch && $(MAKE) $@
	@cd gnullpo && $(MAKE) $@
	@cd grunner && $(MAKE) $@
	@cd gversion && $(MAKE) $@
	@cd mtorz && $(MAKE) $@

install:
	@cd gbball && $(MAKE) $@
	@cd gclock && $(MAKE) $@
	@cd ghebtn && $(MAKE) $@
	@cd glaunch && $(MAKE) $@
	@cd gnullpo && $(MAKE) $@
	@cd grunner && $(MAKE) $@
	@cd gversion && $(MAKE) $@
	@cd mtorz && $(MAKE) $@

clean: touchdepend
	@cd gbball && $(MAKE) $@
	@cd gclock && $(MAKE) $@
	@cd ghebtn && $(MAKE) $@
	@cd glaunch && $(MAKE) $@
	@cd gnullpo && $(MAKE) $@
	@cd grunner && $(MAKE) $@
	@cd gversion && $(MAKE) $@
	@cd mtorz && $(MAKE) $@

depend: touchdepend
	@cd gbball && $(MAKE) $@
	@cd gclock && $(MAKE) $@
	@cd ghebtn && $(MAKE) $@
	@cd glaunch && $(MAKE) $@
	@cd gnullpo && $(MAKE) $@
	@cd grunner && $(MAKE) $@
	@cd gversion && $(MAKE) $@
	@cd mtorz && $(MAKE) $@

distclean:
	rm -f gbball/dependencies
	rm -f gclock/dependencies
	rm -f ghebtn/dependencies
	rm -f glaunch/dependencies
	rm -f gnullpo/dependencies
	rm -f grunner/dependencies
	rm -f gversion/dependencies
	rm -f mtorz/dependencies

touchdepend:
	touch -f gbball/dependencies
	touch -f gclock/dependencies
	touch -f ghebtn/dependencies
	touch -f glaunch/dependencies
	touch -f gnullpo/dependencies
	touch -f grunner/dependencies
	touch -f gversion/dependencies
	touch -f mtorz/dependencies
