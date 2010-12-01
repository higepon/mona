ifndef $(MONADIR)
# export MONADIR=$(PWD)/../mona
export MONADIR=$(shell cd $(PWD)/../../../mona; pwd)
endif

TARGET  = DISPLAY
SOURCES = display.cpp TextArea.cpp

INSTDIR = $(BINDIR)/APPS/BAYGUI
INSTFILES = DISPLAY.map
SHAREDIR   = $(MONADIR)/share
ifneq ($(BUILD_TARGET),ELF)
ADDLINK    =  -lmonalibc-imp -lbaygui-imp --enable-auto-import
ADDLINKDEP = $(MONADIR)/lib/libbaygui-imp.a $(MONADIR)/lib/libmonalibc-imp.a
include $(SHAREDIR)/configs/monapi-ex5.inc
else
ADDLINK    = -lbaygui-imp -lmonalibc-imp -lmonapi-imp
ADDLINKDEP = $(MONADIR)/lib/libbaygui.a $(MONADIR)/lib/libmonalibc.a
include $(SHAREDIR)/configs/monapi-el5.inc
endif

CXXFLAGS += -DDEBUG
INCLUDE += -I .

-include dependencies
