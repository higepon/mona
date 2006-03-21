TARGET  = LABEL
SOURCES = Form1.cpp
INSTDIR = $(BINDIR)/iso/APPS/MONAFRMS

MONADIR    = ../../..
ADDLINK    = -lgui
ADDLINKDEP = $(MONADIR)/lib/libgui.a
ifneq ($(BUILD_TARGET),ELF)
include $(MONADIR)/env/monapi-ex5.inc
else
include $(MONADIR)/env/monapi-el5.inc
endif

CXXFLAGS += -DDEBUG

include dependencies
