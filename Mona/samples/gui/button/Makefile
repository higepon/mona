TARGET  = BUTTON
SOURCES = Form1.cpp
INSTDIR = $(BINDIR)/iso/APPS/MONAFRMS

MONADIR    = ../../..
ADDLINK    = -lgui
ADDLINKDEP = $(MONADIR)/lib/libgui.a
ifneq ($(BUILD_TARGET),ELF)
include $(MONADIR)/env/monapi-ex2.inc
else
include $(MONADIR)/env/monapi-el2.inc
endif

CXXFLAGS += -DDEBUG

include dependencies
