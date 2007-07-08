TARGET  = GIKO
SOURCES = giko.cpp TopLabel.cpp StringHelper.cpp
INSTDIR = $(BINDIR)/APPS/BAYGUI/GIKO.APP
INSTFILES = FAVICON.BMP


SHAREDIR   = $(MONADIR)/share
ifneq ($(BUILD_TARGET),ELF)
ADDLINK    = -lbaygui-imp -lmonalibc-imp -lnet
ADDLINKDEP = $(MONADIR)/lib/libbaygui-imp.a $(MONADIR)/lib/libmonalibc-imp.a
include $(SHAREDIR)/configs/monapi-ex5.inc
else
ADDLINK    = -lbaygui -lmonalibc -lnet
ADDLINKDEP = $(MONADIR)/lib/libbaygui.a $(MONADIR)/lib/libmonalibc.a
include $(SHAREDIR)/configs/monapi-el5.inc
endif

CXXFLAGS += -DDEBUG -nostdinc

-include dependencies
