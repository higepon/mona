TARGET  = BZCAT
SOURCES = main.cpp
INSTDIR = $(BINDIR)/iso/APPS

MONADIR = ../../..
ifneq ($(BUILD_TARGET),ELF)
include $(MONADIR)/env/monapi-ex2.inc
else
include $(MONADIR)/env/monapi-el2.inc
endif

CXXFLAGS += -DDEBUG

include dependencies
