TARGET  = HIGE3D
SOURCES = Form1.cpp notmine.cpp
INSTDIR = $(BINDIR)/iso/APPS/MONAFRMS

MONADIR    = ../../..
ifneq ($(BUILD_TARGET),ELF)
ADDLINK    = -lgui-imp -lmonalibc
include $(MONADIR)/env/monapi-ex5.inc
ADDLINKDEP = $(MONADIR)/lib/libgui-imp.a $(MONADIR)/lib/libmonalibc.a
else
ADDLINK    = -lgui
ADDLINKDEP = $(MONADIR)/lib/libgui.a $(MONADIR)/lib/libmonalibc.a
include $(MONADIR)/env/monapi-el5.inc
endif

CXXFLAGS += -DDEBUG

include dependencies
