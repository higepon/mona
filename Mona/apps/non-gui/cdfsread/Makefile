TARGET  = CDFSREAD
SOURCES = main.cpp ISO9660File.cpp ISO9660FileSystem.cpp IDEDriver.cpp
INSTDIR = $(BINDIR)/iso/APPS

MONADIR = ../../..

ADDLINK    = -lmonalibc
ADDLINKDEP = $(MONADIR)/lib/libmonalibc.a

ifneq ($(BUILD_TARGET),ELF)
include $(MONADIR)/env/monapi-ex2.inc
else
include $(MONADIR)/env/monapi-el2.inc
endif

INCLUDE += -I$(TOOLSDIR)

include dependencies
