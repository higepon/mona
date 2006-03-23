all:
	cd nicdrv && make depend &&  make && make install
	cd monesoc && make depend && make && make install
	cd netprot && make depend &&  make && make install
	cd sampleApp/yamami && make depend && make && make install
	cd sampleApp/ping && make depend && make && make install

clean:
	@cd nicdrv && $(MAKE) $@
	@cd monesoc && $(MAKE) $@
	@cd netprot && $(MAKE) $@
	@cd sampleApp/yamami && $(MAKE) $@
	@cd sampleApp/ping && $(MAKE) $@

uninstall:
	@cd nicdrv && $(MAKE) $@
	@cd monesoc && $(MAKE) $@

