all:
	@cd fat_write && $(MAKE) $@
	@cd bim2bin && $(MAKE) $@
	@cd t5lzma/7zip/Compress/LZMA_Alone && $(MAKE) $@
	@cd bit2mnf && $(MAKE) $@
	@cd monaelf && $(MAKE) $@
	@cd elf_tool && $(MAKE) $@

install:
	@cd fat_write && $(MAKE)
	@cd bim2bin && $(MAKE)
	@cd t5lzma/7zip/Compress/LZMA_Alone && $(MAKE) $@
	@cd bit2mnf && $(MAKE) $@
	@cd monaelf && $(MAKE)
	@cd elf_tool && $(MAKE)

clean: touchdepend
	@cd fat_write && $(MAKE) $@
	@cd bim2bin && $(MAKE) $@
	@cd t5lzma/7zip/Compress/LZMA_Alone && $(MAKE) $@
	@cd bit2mnf && $(MAKE) $@
	@cd monaelf && $(MAKE) $@
	@cd elf_tool && $(MAKE) $@
	@cd fileheader && $(MAKE) $@

depend: touchdepend
	@cd fat_write && $(MAKE) $@
	@cd bim2bin && $(MAKE) $@
	@cd elf_tool && $(MAKE) $@

distclean:
	rm -f fat_write/dependencies
	rm -f bim2bin/dependencies
	rm -f elf_tool/dependencies
	rm -f fileheader/dependencies

touchdepend:
	touch fat_write/dependencies
	touch bim2bin/dependencies
	touch elf_tool/dependencies
	touch fileheader/dependencies
