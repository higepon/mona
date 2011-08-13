ifndef $(MONADIR)
export MONADIR=$(shell cd $(PWD)/../../../; pwd)
endif

TARGET  = TMATH
SOURCES = test.cpp
INSTDIR = $(BINDIR)/APPS/TMATH.APP

SHAREDIR   = $(MONADIR)/share
ADDLINK    =  -lmonalibc-imp --enable-auto-import
ifneq ($(BUILD_TARGET),ELF)
include $(SHAREDIR)/configs/monapi-ex5.inc
else
include $(SHAREDIR)/configs/monapi-el5.inc
endif

CFLAGS += -DDEBUG -I .

-include dependencies
