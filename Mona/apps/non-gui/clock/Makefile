TARGET  = CLOCK
SOURCES = clock.cpp
INSTDIR = $(BINDIR)/iso/APPS

MONADIR = ../../..
ifneq ($(BUILD_TARGET),ELF)
include $(MONADIR)/env/monapi-ex5.inc
else
include $(MONADIR)/env/monapi-el5.inc
endif

include dependencies
