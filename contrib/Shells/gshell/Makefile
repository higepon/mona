TARGET  = GSHELL
SOURCES = gshell.cpp
INSTDIR = $(BINDIR)/APPS/MONAGUI

SHAREDIR   = $(MONADIR)/share
ifneq ($(BUILD_TARGET),ELF)
ADDLINK    = -lamonagui-imp -lmonalibc-imp
ADDLINKDEP = $(MONADIR)/lib/libamonagui-imp.a $(MONADIR)/lib/libmonalibc-imp.a
include $(SHAREDIR)/configs/monapi-ex5.inc
else
ADDLINK    = -lmonagui -lmonalibc
ADDLINKDEP = $(MONADIR)/lib/libamonagui.a $(MONADIR)/lib/libmonalibc.a
include $(SHAREDIR)/configs/monapi-el5.inc
endif

CXXFLAGS += -DDEBUG

-include dependencies
