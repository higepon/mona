TARGET  = BITMAP
SOURCES = Form1.cpp

INSTDIR = $(BINDIR)/APPS/MONAFRMS
SHAREDIR   = $(MONADIR)/share
ifneq ($(BUILD_TARGET),ELF)
ADDLINK    = -lgui-imp
ADDLINKDEP = $(MONADIR)/lib/libgui-imp.a
include $(SHAREDIR)/configs/monapi-ex5.inc
else
ADDLINK    = -lgui
ADDLINKDEP = $(MONADIR)/lib/lgui.a
include $(SHAREDIR)/configs/monapi-el5.inc
endif

CXXFLAGS += -DDEBUG
INCLUDE += -I .

-include dependencies
