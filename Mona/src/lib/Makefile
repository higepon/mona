all:
	@cd monapi; $(MAKE) $@
	@cd monalibc; $(MAKE) $@
	@cd gui; $(MAKE) $@
	@cd jpegls; $(MAKE) $@
	@cd bzip2; $(MAKE) $@
	@cd baygui; $(MAKE) $@

install:
	@cd monapi; $(MAKE) $@
	@cd monalibc; $(MAKE) $@
	@cd gui; $(MAKE) $@
	@cd jpegls; $(MAKE) $@
	@cd bzip2; $(MAKE) $@
	@cd baygui; $(MAKE) $@

clean: touchdepend
	@cd monapi; $(MAKE) $@
	@cd monalibc; $(MAKE) $@
	@cd gui; $(MAKE) $@
	@cd jpegls; $(MAKE) $@
	@cd bzip2; $(MAKE) $@
	@cd baygui; $(MAKE) $@
	rm -f ../../lib/*.a

depend: touchdepend
	@cd monapi; $(MAKE) $@
	@cd monalibc; $(MAKE) $@
	@cd gui; $(MAKE) $@
	@cd jpegls; $(MAKE) $@
	@cd bzip2; $(MAKE) $@
	@cd baygui; $(MAKE) $@

distclean:
	rm -f monapi/dependencies
	rm -f monalibc/dependencies
	rm -f gui/dependencies
	rm -f jpegls/dependencies
	rm -f bzip2/dependencies
	rm -f baygui/dependencies

touchdepend:
	touch monapi/dependencies
	touch monalibc/dependencies
	touch gui/dependencies
	touch jpegls/dependencies
	touch bzip2/dependencies
	touch baygui/dependencies
