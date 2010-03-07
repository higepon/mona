ifndef $(MONADIR)
export MONADIR=$(shell cd $(PWD)/../../../; pwd)
endif

TARGET  = TMSG
SOURCES = message.cpp
INSTDIR = $(BINDIR)/APPS

SHAREDIR   = $(MONADIR)/share
ADDLINK    =  -lmonalibc-imp --enable-auto-import
ifneq ($(BUILD_TARGET),ELF)
include $(SHAREDIR)/configs/monapi-ex5.inc
else
include $(SHAREDIR)/configs/monapi-el5.inc
endif

CFLAGS += -DDEBUG -I . -I $(INCDIR)/monalibc -mno-stack-arg-probe -g

-include dependencies
