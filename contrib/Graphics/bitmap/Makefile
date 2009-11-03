TARGET  = BITMAP
SOURCES = Form1.cpp

INSTDIR = $(BINDIR)/APPS/MONAFRMS
INSTFILES  = MONALISA.JPG SAKEBI.JPG
SHAREDIR   = $(MONADIR)/share
ifneq ($(BUILD_TARGET),ELF)
ADDLINK    = -lgui-imp -lmonalibc-imp --enable-auto-import
ADDLINKDEP = $(MONADIR)/lib/libgui-imp.a $(MONADIR)/lib/libmonalibc-imp.a
include $(SHAREDIR)/configs/monapi-ex5.inc
else
ADDLINK    = -lgui
ADDLINKDEP = $(MONADIR)/lib/lgui.a
include $(SHAREDIR)/configs/monapi-el5.inc
endif

CXXFLAGS += -DDEBUG
INCLUDE += -I . -I $(INCDIR)/monalibc

-include dependencies
