TARGET  = YAMAMI
SOURCES = main.cpp
INSTDIR = $(BINDIR)/root/APPS

ADDLINK    = -lPci -lmonalibc
ADDLINKDEP = $(MONADIR)/lib/libmonalibc.a
MONADIR = ../../..
ifneq ($(BUILD_TARGET),ELF)
include $(MONADIR)/env/monapi-ex2.inc
else
include $(MONADIR)/env/monapi-el2.inc
endif

INCLUDE += -I$(TOOLSDIR)

include dependencies
