all:
	@cd jhello && $(MAKE) $@
	@cd jnoiz2bg && $(MAKE) $@
	@cd java3d01 && $(MAKE) $@
	@cd java3d02 && $(MAKE) $@
	@cd java3d03 && $(MAKE) $@
	@cd java3d04 && $(MAKE) $@
	@cd java3d13 && $(MAKE) $@

install:
	@cd jhello && $(MAKE) $@
	@cd jnoiz2bg && $(MAKE) $@
	@cd java3d01 && $(MAKE) $@
	@cd java3d02 && $(MAKE) $@
	@cd java3d03 && $(MAKE) $@
	@cd java3d04 && $(MAKE) $@
	@cd java3d13 && $(MAKE) $@

clean: touchdepend
	@cd jhello && $(MAKE) $@
	@cd jnoiz2bg && $(MAKE) $@
	@cd java3d01 && $(MAKE) $@
	@cd java3d02 && $(MAKE) $@
	@cd java3d03 && $(MAKE) $@
	@cd java3d04 && $(MAKE) $@
	@cd java3d13 && $(MAKE) $@

depend: touchdepend
	@cd jhello && $(MAKE) $@
	@cd jnoiz2bg && $(MAKE) $@
	@cd java3d01 && $(MAKE) $@
	@cd java3d02 && $(MAKE) $@
	@cd java3d03 && $(MAKE) $@
	@cd java3d04 && $(MAKE) $@
	@cd java3d13 && $(MAKE) $@

distclean:
	rm -f jhello/dependencies
	rm -f jnoiz2bg/dependencies
	rm -f java3d01/dependencies
	rm -f java3d02/dependencies
	rm -f java3d03/dependencies
	rm -f java3d04/dependencies
	rm -f java3d13/dependencies

touchdepend:
	touch jhello/dependencies
	touch jnoiz2bg/dependencies
	touch java3d01/dependencies
	touch java3d02/dependencies
	touch java3d03/dependencies
	touch java3d04/dependencies
	touch java3d13/dependencies
