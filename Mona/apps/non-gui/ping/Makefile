TARGET  = PING
SOURCES = main.cpp
INSTDIR = $(BINDIR)/iso/APPS

ADDLINK    = -lmonalibc
MONADIR = ../../..
ifneq ($(BUILD_TARGET),ELF)
include $(MONADIR)/env/monapi-ex5.inc
else
include $(MONADIR)/env/monapi-el5.inc
endif

INCLUDE += -I$(TOOLSDIR)

include dependencies
