ifndef $(MONADIR)
export MONADIR=$(shell cd $(PWD)/../../../../mona; pwd)
endif

TARGET  = WGET
SOURCES = main.cpp
INSTDIR = $(BINDIR)/APPS

SHAREDIR   = $(MONADIR)/share
ADDLINK    = -lmonalibc-imp
ifneq ($(BUILD_TARGET),ELF)
include $(SHAREDIR)/configs/monapi-ex5.inc
else
include $(SHAREDIR)/configs/monapi-el5.inc
endif

-include dependencies
