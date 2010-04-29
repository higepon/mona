SRCTOP=.
include $(SRCTOP)/Makefile.inc

PREFIX=$(MONADIR)/local
PATH:=$(PATH):$(PREFIX)/bin
export PATH
export MINGWPREFIX

#temp support for win32
AR=ar r 
INSTALL=install.exe
EXEEXT=.exe
RANLIB=ranlib
NASM=nasm.exe
STRIP=strip.exe
export INSTALL
export EXEEXT
export AR
export RANLIB
export NASM
export STRIP



COREDIR = core/monalibc core/firstboot

all: mkimg
tools:
	cd tool/stlport && $(MAKE) && $(MAKE) install
	cd tool/fat_write && $(MAKE) && $(MAKE) install
	cd tool/t5lzma && $(MAKE) && $(MAKE) install
	cd tool/bim2bin && $(MAKE) && $(MAKE) install
	cd external/libjpegls && $(MAKE) && $(MAKE) install

externals: cores
	cd external/onig && $(MAKE) && $(MAKE) install

cores: tools
	cd core/firstboot && $(MAKE) && $(MAKE) install
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
	cd core/libpci && $(MAKE) && $(MAKE) install
	cd core/net_server && $(MAKE) && $(MAKE) install
	cd core/keyboard_server && $(MAKE) && $(MAKE) install
	cd core/mouse_server && $(MAKE) && $(MAKE) install
	cd core/file_server && $(MAKE) && $(MAKE) install
	cd core/process_server && $(MAKE) && $(MAKE) install
	cd core/shell_server && $(MAKE) && $(MAKE) install
	cd core/elf_server && $(MAKE) && $(MAKE) install
	cd core/pe_server && $(MAKE) && $(MAKE) install
	cd core/gui_server && $(MAKE) && $(MAKE) install
	cd core/scheme && $(MAKE) -f Makefile.mona && $(MAKE) -f Makefile.mona install

tests: tools cores
	cd test/monalibc/string && $(MAKE) && $(MAKE) install
	cd test/monalibc/stdio && $(MAKE) && $(MAKE) install
	cd test/monapi/mutex && $(MAKE) && $(MAKE) install
	cd test/monapi/condition && $(MAKE) && $(MAKE) install
	cd test/monapi/misc && $(MAKE) && $(MAKE) install
	cd test/monapi/message && $(MAKE) && $(MAKE) install
	cd test/monapi/socket && $(MAKE) && $(MAKE) install
	cd test/monapi/ram_disk && $(MAKE) && $(MAKE) install

mkimg: cores externals tools tests
	cd tool/mkimg && $(MAKE) clean && $(MAKE)

depend: clean
	cd core/kernel && $(MAKE) $@
	cd core/monapi && $(MAKE) $@
	cd core/libpci && $(MAKE) $@
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
	cd core/gui_server && $(MAKE) $@
	cd test/monalibc/string && $(MAKE) $@
	cd test/monalibc/stdio && $(MAKE) $@

.PHONY: clean doc

doc:
	perl ~/NaturalDocs-1.4/NaturalDocs -i . -o HTML doc/html -p doc/config -s pink

# 1. run qemu as daemon.
# 2. wait for test results are written in /tmp/mona_serial.log.
# 3. check test results.
# 4. kill qemu by pid.
check:
	@tool/qemu/as_daemon.sh
	@mosh tool/qemu/check_tests.scm

clean:
	$(MAKE) $@ -C core/firstboot
ifdef CSC
	$(MAKE) $@ -C core/PEAnalyzerLib
	$(MAKE) $@ -C core/IL2Asm16
endif
	$(MAKE) $@ -C external/onig
	$(MAKE) $@ -C core/secondboot
	$(MAKE) $@ -C core/kernel
	$(MAKE) $@ -C core/monapi
	$(MAKE) $@ -C core/libpci
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
	$(MAKE) $@ -C core/gui_server
	$(MAKE) $@ -C core/scheme -f Makefile.mona
	cd test/monalibc/string && make clean
	cd test/monalibc/stdio && make clean
	cd test/monapi/mutex && make clean
	cd test/monapi/condition && make clean
