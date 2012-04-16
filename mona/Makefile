SRCTOP=.
include $(SRCTOP)/Makefile.inc

PREFIX=$(MONADIR)/local
PATH:=$(PATH):$(PREFIX)/bin
export PATH
export MINGWPREFIX

COREDIR = core/monalibc core/firstboot

all: mkimg

include/sys/error.h: include/sys/error.txt
	mosh tool/gen-error-h.scm $< > $@

core/monapi/error.cpp: include/sys/error.txt
	mosh tool/gen-error-cpp.scm $< > $@

tools: include/sys/error.h core/monapi/error.cpp
	cd tool/fat_write && $(MAKE) && $(MAKE) install
	cd tool/t5lzma && $(MAKE) && $(MAKE) install
	cd tool/bim2bin && $(MAKE) && $(MAKE) install
	cd external/libjpegls && $(MAKE) && $(MAKE) install

externals: cores
	cd external/onig && $(MAKE) && $(MAKE) install

cores: tools
	mkdir -p $(BINDIR)/USER
	cd core/firstboot && $(MAKE) && $(MAKE) install
ifdef CSC
	cd core/PEAnalyzerLib && $(MAKE) && $(MAKE) install
	cd core/IL2Asm16 && $(MAKE) && $(MAKE) install
endif
	cd core/secondboot && $(MAKE) && $(MAKE) install
	cd core/kernel && $(MAKE) && $(MAKE) install
	cd core/monapi && $(MAKE) && $(MAKE) install
	cd core/monalibc && $(MAKE) && $(MAKE) install
	cd core/openssl-1.0.0c && touch Makefile && MAKEFLAGS=-j1 $(MAKE) ; $(MAKE) install-mona #
	cd core/monitor_server && $(MAKE) && $(MAKE) install
	cd core/screen_server && $(MAKE) && $(MAKE) install
	cd core/net_server && $(MAKE) && $(MAKE) install
	cd core/keyboard_server && $(MAKE) && $(MAKE) install
	cd core/mouse_server && $(MAKE) && $(MAKE) install
	cd core/file_server && $(MAKE) && $(MAKE) install
	cd core/process_server && $(MAKE) && $(MAKE) install
	cd core/shell_server && $(MAKE) && $(MAKE) install
	cd core/elf_server && $(MAKE) && $(MAKE) install
	cd core/pe_server && $(MAKE) && $(MAKE) install
	cd core/gui_server && $(MAKE) && $(MAKE) install
	cd core/clipboard_server && $(MAKE) && $(MAKE) install
	cd core/scheme && $(MAKE) -f Makefile.mona && $(MAKE) -f Makefile.mona install
	cd core/gmp && $(MAKE) && $(MAKE) install
	cd core/zlib && $(MAKE) && $(MAKE) install
	cd core/libpng && $(MAKE) && $(MAKE) install
	cd core/libjpeg && $(MAKE) && $(MAKE) install
	cd core/icu && $(MAKE) && $(MAKE) install
	cd core/mosh && $(MAKE) -f Makefile.mona && $(MAKE) -f Makefile.mona install

tests: tools cores
	cd test/monapi/mosh && $(MAKE) && $(MAKE) install
	cd test/monapi/wait && $(MAKE) && $(MAKE) install
	cd test/monapi/process_server && $(MAKE) && $(MAKE) install
	cd test/monapi/file && $(MAKE) && $(MAKE) install
	cd test/monapi/file_load && $(MAKE) && $(MAKE) install
	cd test/monalibc/string && $(MAKE) && $(MAKE) install
	cd test/monalibc/stdio && $(MAKE) && $(MAKE) install
	cd test/monalibc/stdlib && $(MAKE) && $(MAKE) install
	cd test/monalibc/math && $(MAKE) && $(MAKE) install
	cd test/monapi/mutex && $(MAKE) && $(MAKE) install
	cd test/monapi/stack_trace && $(MAKE) && $(MAKE) install
	cd test/monapi/condition && $(MAKE) && $(MAKE) install
	cd test/monapi/misc && $(MAKE) && $(MAKE) install
	cd test/monapi/message && $(MAKE) && $(MAKE) install
	cd test/monapi/socket && $(MAKE) && $(MAKE) install
	cd test/monapi/ram_disk && $(MAKE) && $(MAKE) install
	cd test/monapi/die && $(MAKE) && $(MAKE) install
	cd test/monapi/ram_disk && $(MAKE) && $(MAKE) install
	cd test/monapi/syscall && $(MAKE) && $(MAKE) install
	cd test/monapi/clipboard && $(MAKE) && $(MAKE) install
	cd test/monapi/stream && $(MAKE) && $(MAKE) install
	cd test/monapi/stream-sub && $(MAKE) && $(MAKE) install
	cd test/monapi/empty && $(MAKE) && $(MAKE) install
	cd test/ext/gmp && $(MAKE) && $(MAKE) install
	cd test/drivers/virtio && $(MAKE) && $(MAKE) install
	cd test/drivers/block_cache && $(MAKE) && $(MAKE) install
mkimg: cores externals tools tests
	cd tool/mkimg && $(MAKE) clean && $(MAKE)

without-mkimg: cores externals tools tests


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

check-one:
	@tool/qemu/as_daemon.sh
	@mosh tool/qemu/check_tests.scm $(TEST_NAME)


TARGETS = \
core/clipboard_server \
core/elf_server \
core/file_server \
core/gmp \
core/gui_server \
core/kernel \
core/keyboard_server \
core/libpci \
core/monalibc \
core/monapi \
core/monitor_server \
core/mouse_server \
core/net_server \
core/pe_server \
core/process_server \
core/screen_server \
core/secondboot \
core/libpng \
core/icu \
core/zlib \
core/shell_server \
external/libjpegls \
external/onig \
test/drivers/virtio \
test/drivers/block_cache \
test/ext/gmp \
test/monalibc/math \
test/monalibc/stdio \
test/monalibc/stdlib \
test/monalibc/string \
test/monapi/clipboard \
test/monapi/condition \
test/monapi/die \
test/monapi/file \
test/monapi/file_load \
test/monapi/message \
test/monapi/misc \
test/monapi/mutex \
test/monapi/ram_disk \
test/monapi/socket \
test/monapi/stack_trace \
test/monapi/syscall \
test/monapi/mosh \
test/monapi/wait \
test/monapi/empty \
test/monapi/process_server \
test/monapi/stream-sub \
test/monapi/stream \
tool/fat_write \
tool/messageHeader \
tool/t5lzma

export MONADIR

clean:
	cd core/monapi/ && $(MAKE) uninstall
	$(MAKE) $@ -C core/scheme -f Makefile.mona
	$(MAKE) $@ -C core/mosh -f Makefile.mona
ifdef CSC
	$(MAKE) $@ -C core/PEAnalyzerLib
	$(MAKE) $@ -C core/IL2Asm16
endif
	for i in `echo $(TARGETS)`; do (cd $$i && $(MAKE) $@) ;done

depend:
	cd core/scheme && $(MAKE) -f Makefile.mona $@
	cd core/mosh && $(MAKE) -f Makefile.mona $@
	for i in `echo $(TARGETS)`; do (cd $$i && $(MAKE) $@) ;done
