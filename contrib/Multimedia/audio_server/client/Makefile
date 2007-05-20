ifndef $(MONADIR)
# export MONADIR=$(PWD)/../mona
export MONADIR=$(shell cd $(PWD)/../../../../mona; pwd)
endif

TARGET  = ACLI
SOURCES = main.cpp
INSTDIR = $(BINDIR)/APPS
INSTFILES = test.wav

SHAREDIR   = $(MONADIR)/share
ADDLINK    = -lmonalibc-imp
ifneq ($(BUILD_TARGET),ELF)
include $(SHAREDIR)/configs/monapi-ex5.inc
else
include $(SHAREDIR)/configs/monapi-el5.inc
endif

CXXFLAGS += -DDEBUG
INCLUDE += -I$(INCDIR)/stlport -I$(INCDIR)/monalibc

include dependencies
