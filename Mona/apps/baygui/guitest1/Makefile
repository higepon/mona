TARGET  = GUITEST1
SOURCES = guitest1.cpp
INSTDIR = $(BINDIR)/iso/APPS/BAYGUI

MONADIR    = ../../..
ifneq ($(BUILD_TARGET),ELF)
ADDLINK    = -lbaygui-imp
ADDLINKDEP = $(MONADIR)/lib/libbaygui-imp.a
include $(MONADIR)/env/monapi-ex5.inc
else
ADDLINK    = -lbaygui
ADDLINKDEP = $(MONADIR)/lib/libbaygui.a
include $(MONADIR)/env/monapi-el5.inc
endif

CXXFLAGS += -DDEBUG

include dependencies
