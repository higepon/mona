ifndef $(MONADIR)
# export MONADIR=$(PWD)/../mona
export MONADIR=$(shell cd $(PWD)/../../../mona; pwd)
endif

TARGET  = HELLO
SOURCES = main.cpp
INSTDIR = $(BINDIR)/APPS

SHAREDIR   = $(MONADIR)/share
ADDLINK    =  -lonigurum-imp -lmonalibc-imp -lpci-imp
#ADDLINK    = -ldlltest-imp
ifneq ($(BUILD_TARGET),ELF)
include $(SHAREDIR)/configs/monapi-ex5.inc
else
include $(SHAREDIR)/configs/monapi-el5.inc
endif

CXXFLAGS += -g -DDEBUG -I . -I ./boost -I $(INCDIR)/onig

-include dependencies
