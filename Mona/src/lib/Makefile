all:
	@cd monapi && $(MAKE) $@
	@cd monalibc && $(MAKE) $@
	@cd gui && $(MAKE) $@
	@cd jpegls && $(MAKE) $@
	@cd bzip2 && $(MAKE) $@
	@cd baygui && $(MAKE) $@
	@cd pci && $(MAKE) $@
	@cd monesoc&& $(MAKE) $@

install:
	@cd monapi && $(MAKE) $@
	@cd monalibc && $(MAKE) $@
	@cd gui && $(MAKE) $@
	@cd jpegls && $(MAKE) $@
	@cd bzip2 && $(MAKE) $@
	@cd baygui && $(MAKE) $@
	@cd pci && $(MAKE) $@
	@cd monesoc&& $(MAKE) $@

clean: touchdepend
	@cd monapi && $(MAKE) $@
	@cd monalibc && $(MAKE) $@
	@cd gui && $(MAKE) $@
	@cd jpegls && $(MAKE) $@
	@cd bzip2 && $(MAKE) $@
	@cd baygui && $(MAKE) $@
	@cd pci && $(MAKE) $@
	@cd monesoc&& $(MAKE) $@	
	rm -f ../../lib/*.a

depend: touchdepend
	@cd monapi && $(MAKE) $@
	@cd monalibc && $(MAKE) $@
	@cd gui && $(MAKE) $@
	@cd jpegls && $(MAKE) $@
	@cd bzip2 && $(MAKE) $@
	@cd baygui && $(MAKE) $@
	@cd pci && $(MAKE) $@
	@cd monesoc&& $(MAKE) $@

distclean:
	rm -f monapi/dependencies
	rm -f monalibc/dependencies
	rm -f gui/dependencies
	rm -f jpegls/dependencies
	rm -f bzip2/dependencies
	rm -f baygui/dependencies
	rm -f pci/dependencies
	rm -f pci/monesoc

touchdepend:
	touch monapi/dependencies
	touch monalibc/dependencies
	touch gui/dependencies
	touch jpegls/dependencies
	touch bzip2/dependencies
	touch baygui/dependencies
	touch pci/dependencies
	touch monesoc/dependencies
