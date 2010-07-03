ifndef $(MONADIR)
export MONADIR=$(shell cd $(PWD)/../../../; pwd)
endif

TARGET  = TSTACK
SOURCES = stack_trace.cpp
INSTDIR = $(BINDIR)/APPS/TSTACK.APP
INSTFILES = TEST.map
$(TARGET).EX5: $(INSTFILES)

SHAREDIR   = $(MONADIR)/share
ADDLINK    =  -lmonalibc-imp --enable-auto-import
ifneq ($(BUILD_TARGET),ELF)
include $(SHAREDIR)/configs/monapi-ex5.inc
else
include $(SHAREDIR)/configs/monapi-el5.inc
endif

CFLAGS += -DDEBUG -I . -I $(INCDIR)/monalibc

-include dependencies
