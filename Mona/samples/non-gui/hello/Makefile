TARGET  = HELLO
SOURCES = hello.cpp
INSTDIR = $(BINDIR)/root/APPS

MONADIR = ../../..
ifneq ($(BUILD_TARGET),ELF)
include $(MONADIR)/env/monapi-ex2.inc
else
include $(MONADIR)/env/monapi-el2.inc
endif

ADDLINK = -lmonalibc

include dependencies
