all:
	@cd noiz2bg; $(MAKE) $@
	@cd reversi; $(MAKE) $@
	@cd jpegdemo; $(MAKE) $@
#	@cd clock; $(MAKE) $@
	@cd nic; $(MAKE) $@
	@cd waba; $(MAKE) $@
	@cd tube; $(MAKE) $@
	@cd bzcat; $(MAKE) $@
	@cd monamona; $(MAKE) $@
	@cd pciinfo; $(MAKE) $@

install:
	@cd noiz2bg; $(MAKE) $@
	@cd reversi; $(MAKE) $@
	@cd jpegdemo; $(MAKE) $@
#	@cd clock; $(MAKE) $@
	@cd nic; $(MAKE) $@
	@cd waba; $(MAKE) $@
	@cd tube; $(MAKE) $@
	@cd bzcat; $(MAKE) $@
	@cd monamona; $(MAKE) $@
	@cd pciinfo; $(MAKE) $@

clean: touchdepend
	@cd noiz2bg; $(MAKE) $@
	@cd reversi; $(MAKE) $@
	@cd jpegdemo; $(MAKE) $@
#	@cd clock; $(MAKE) $@
	@cd nic; $(MAKE) $@
	@cd waba; $(MAKE) $@
	@cd tube; $(MAKE) $@
	@cd bzcat; $(MAKE) $@
	@cd monamona; $(MAKE) $@
	@cd pciinfo; $(MAKE) $@

depend: touchdepend
	@cd noiz2bg; $(MAKE) $@
	@cd reversi; $(MAKE) $@
	@cd jpegdemo; $(MAKE) $@
#	@cd clock; $(MAKE) $@
	@cd nic; $(MAKE) $@
	@cd waba; $(MAKE) $@
	@cd bzcat; $(MAKE) $@
	@cd monamona; $(MAKE) $@
	@cd pciinfo; $(MAKE) $@

distclean:
	rm -f noiz2bg/dependencies
	rm -f reversi/dependencies
	rm -f jpegdemo/dependencies
#	rm -f clock/dependencies
	rm -f waba/dependencies
	rm -f bzcat/dependencies
	rm -f monamona/dependencies
	rm -f pciinfo/dependencies
	@cd nic; $(MAKE) $@

touchdepend:
	touch noiz2bg/dependencies
	touch reversi/dependencies
	touch jpegdemo/dependencies
#	touch clock/dependencies
	touch waba/dependencies
	touch bzcat/dependencies
	touch monamona/dependencies
	touch pciinfo/dependencies
