ifndef $(MONADIR)
export MONADIR=$(PWD)/../../../mona
endif

TARGET  = STDOUT
SOURCES = main.cpp
INSTDIR = $(BINDIR)/APPS

SHAREDIR   = $(MONADIR)/share
ifneq ($(BUILD_TARGET),ELF)
include $(SHAREDIR)/configs/monapi-ex5.inc
ADDLINK =  -lmonalibc-imp
else
include $(SHAREDIR)/configs/monapi-el5.inc
ADDLINK =  -lmonalibc-imp
endif

CXXFLAGS += -DDEBUG

include dependencies
