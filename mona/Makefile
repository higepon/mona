SRCTOP=.
include $(SRCTOP)/Makefile.inc

PREFIX=$(MONADIR)/local
PATH:=$(PATH):$(PREFIX)/bin
export PATH
export MINGWPREFIX

COREDIR = core/monalibc core/firstboot

all: tools cores externals
tools:
	cd tool/stlport && $(MAKE) && $(MAKE) install
	cd tool/fat_write && $(MAKE) && $(MAKE) install
	cd tool/t5lzma && $(MAKE) && $(MAKE) install
	cd tool/bim2bin && $(MAKE) && $(MAKE) install

externals:
	cd external/onig && $(MAKE) && $(MAKE) install

cores:
	cd core/firstboot && $(MAKE) && $(MAKE) install
#ifneq ($(SKIP_DOTNET),1)
ifdef CSC
	cd core/PEAnalyzerLib && $(MAKE) && $(MAKE) install
	cd core/IL2Asm16 && $(MAKE) && $(MAKE) install
endif
	cd core/secondboot && $(MAKE) && $(MAKE) install
	cd core/kernel && $(MAKE) && $(MAKE) install
	cd core/monapi && $(MAKE) && $(MAKE) install
	cd core/monalibc && $(MAKE) && $(MAKE) install
	cd core/monitor_server && $(MAKE) && $(MAKE) install
	cd core/screen_server && $(MAKE) && $(MAKE) install
	cd core/keyboard_server && $(MAKE) && $(MAKE) install
	cd core/mouse_server && $(MAKE) && $(MAKE) install
	cd core/file_server && $(MAKE) && $(MAKE) install
	cd core/process_server && $(MAKE) && $(MAKE) install
	cd core/shell_server && $(MAKE) && $(MAKE) install
	cd core/elf_server && $(MAKE) && $(MAKE) install
	cd core/pe_server && $(MAKE) && $(MAKE) install
	cd core/test && $(MAKE) && $(MAKE) install
	cd tool/mkimg && $(MAKE) clean && $(MAKE)

depend: clean
	cd core/kernel && $(MAKE) $@
	cd core/monapi && $(MAKE) $@
	cd core/monalibc && $(MAKE) $@
	cd core/monitor_server && $(MAKE) $@
	cd core/screen_server && $(MAKE) $@
	cd core/keyboard_server && $(MAKE) $@
	cd core/mouse_server && $(MAKE) $@
	cd core/file_server && $(MAKE) $@
	cd core/process_server && $(MAKE) $@
	cd core/shell_server && $(MAKE) $@
	cd core/elf_server && $(MAKE) $@&
	cd core/pe_server && $(MAKE) $@
	cd core/test && $(MAKE) $@


.PHONY: clean
clean:
	$(MAKE) $@ -C core/firstboot
#ifneq ($(SKIP_DOTNET),1)
ifdef CSC
	$(MAKE) $@ -C core/PEAnalyzerLib
	$(MAKE) $@ -C core/IL2Asm16
endif
	$(MAKE) $@ -C external/onig
	$(MAKE) $@ -C core/secondboot
	$(MAKE) $@ -C core/kernel
	$(MAKE) $@ -C core/monapi
	$(MAKE) $@ -C core/monitor_server
	$(MAKE) $@ -C core/screen_server
	$(MAKE) $@ -C core/keyboard_server
	$(MAKE) $@ -C core/monalibc
	$(MAKE) $@ -C core/mouse_server
	$(MAKE) $@ -C core/file_server
	$(MAKE) $@ -C core/process_server
	$(MAKE) $@ -C core/shell_server
	$(MAKE) $@ -C core/elf_server
	$(MAKE) $@ -C core/pe_server
	$(MAKE) $@ -C core/test
 
