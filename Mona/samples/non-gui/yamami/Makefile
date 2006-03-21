TARGET  = YAMAMI
SOURCES = main.cpp

INSTDIR = $(BINDIR)/iso/APPS

ADDLINK    = -lmonesoc -lPci -lmonalibc
ADDLINKDEP = $(MONADIR)/lib/libmonalibc.a
MONADIR = ../../..
ifneq ($(BUILD_TARGET),ELF)
include $(MONADIR)/env/monapi-ex5.inc
else
include $(MONADIR)/env/monapi-el5.inc
endif

INCLUDE += -I$(TOOLSDIR)

include dependencies
