TARGET  = GPS
SOURCES = Form1.cpp
INSTDIR = $(BINDIR)/root/APPS

MONADIR    = ../../..
ADDLINK    = -lmonalibc
ADDLINKDEP = $(MONADIR)/lib/libmonalibc.a
#ifneq ($(BUILD_TARGET),ELF)
#ADDLINK    += -lgui-imp
#ADDLINKDEP += $(MONADIR)/lib/libgui-imp.a
#include $(MONADIR)/env/monapi-ex2.inc
#else
ADDLINK    += -lgui
ADDLINKDEP += $(MONADIR)/lib/libgui.a
include $(MONADIR)/env/monapi-el2.inc
#endif

CXXFLAGS += -DDEBUG

include dependencies
