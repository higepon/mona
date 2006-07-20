SRCTOP=.
include $(SRCTOP)/Makefile.inc

PREFIX=$(MONADIR)/local
PATH:=$(PATH):$(PREFIX)/bin
export PATH
export MINGWPREFIX

all: tools cores
tools:
	cd tool/fat_write && make && make install
	cd tool/t5lzma && make && make install
	cd tool/bim2bin && make && make install
	
cores:
	cd core/firstboot && make && make install
	cd core/PEAnalyzerLib && make && make install
	cd core/IL2Asm16 && make && make install
	cd core/secondboot && make && make install
	cd core/kernel && make && make install
	cd core/monapi && make && make install
	cd core/monitor_server && make && make install
	cd core/keyboard_server && make && make install
	cd core/monalibc && make && make install
	cd core/mouse_server && make && make install
	cd core/file_server && make && make install
	cd core/process_server && make && make install
	cd core/shell_server && make && make install
	cd core/elf_server && make && make install
	cd core/pe_server && make && make install
	cd tool/mkimg && make clean && make

clean:
	
