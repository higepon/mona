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

depend: touchdepend
	@cd monapi; $(MAKE) $@
	@cd monalibc; $(MAKE) $@
	@cd gui; $(MAKE) $@
	@cd jpegls; $(MAKE) $@

distclean:
	rm -f monapi/dependencies
	rm -f monalibc/dependencies
	rm -f gui/dependencies
	rm -f jpegls/dependencies

touchdepend:
	touch monapi/dependencies
	touch monalibc/dependencies
	touch gui/dependencies
	touch jpegls/dependencies
