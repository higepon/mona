ifndef $(MONADIR)
# export MONADIR=$(PWD)/../mona
export MONADIR=$(shell cd $(PWD)/../../../mona; pwd)
endif

TARGET  = HELLO
SOURCES = main.cpp
INSTDIR = $(BINDIR)/APPS
INSTFILES = HELLO.map
ADDINCLUDE= 

SHAREDIR   = $(MONADIR)/share
ADDLINK    =   -lssl-imp -lcrypto-imp -lonigurum-imp -lmonalibc-imp --enable-auto-import
#ADDLINK    = -ldlltest-imp
ifneq ($(BUILD_TARGET),ELF)
include $(SHAREDIR)/configs/monapi-ex5.inc
else
include $(SHAREDIR)/configs/monapi-el5.inc
endif

CXXFLAGS += -g -DDEBUG -I . -I ./boost -I $(INCDIR)/onig

-include dependencies
