all:
	@cd monapi; $(MAKE) $@
	@cd monalibc; $(MAKE) $@
	@cd gui; $(MAKE) $@
	@cd jpegls; $(MAKE) $@

install:
	@cd monapi; $(MAKE) $@
	@cd monalibc; $(MAKE) $@
	@cd gui; $(MAKE) $@
	@cd jpegls; $(MAKE) $@

clean: touchdepend
	@cd monapi; $(MAKE) $@
	@cd monalibc; $(MAKE) $@
	@cd gui; $(MAKE) $@
	@cd jpegls; $(MAKE) $@
	rm -f ../../lib/*.a

distclean:
	rm -f monapi/dependencies
	rm -f monalibc/dependencies
	rm -f gui/dependencies
	rm -f jpegls/dependencies

depend: touchdepend
	@cd monapi; $(MAKE) $@
	@cd monalibc; $(MAKE) $@
	@cd gui; $(MAKE) $@
	@cd jpegls; $(MAKE) $@

touchdepend:
	touch monapi/dependencies
	touch monalibc/dependencies
	touch gui/dependencies
	touch jpegls/dependencies
