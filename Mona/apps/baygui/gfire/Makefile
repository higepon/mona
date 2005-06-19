TARGET  = GFIRE
SOURCES = gfire.cpp
NASMSRCS = fire.asm
INSTDIR = $(BINDIR)/iso/APPS/BAYGUI

MONADIR    = ../../..
#ifneq ($(BUILD_TARGET),ELF)
#ADDLINK    = -lbaygui-imp
#ADDLINKDEP = $(MONADIR)/lib/libbaygui-imp.a
#include $(MONADIR)/env/monapi-ex5.inc
#else
ADDLINK    = -lbaygui
ADDLINKDEP = $(MONADIR)/lib/libbaygui.a
include $(MONADIR)/env/monapi-el5.inc
.SUFFIXES: .asm .o
.asm.o:
	$(NASM) $(NFLAGS) -o $@ -fwin32 $<

#endif

CXXFLAGS += -DDEBUG

include dependencies
