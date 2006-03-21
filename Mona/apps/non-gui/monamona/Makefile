TARGET  = MONAMONA
SOURCES = main.cpp \
          BfMain.cpp
INSTDIR = $(BINDIR)/iso/APPS

MONADIR    = ../../..
ADDLINK    = -lmonalibc
ADDLINKDEP = $(MONADIR)/lib/libmonalibc.a
ifneq ($(BUILD_TARGET),ELF)
include $(MONADIR)/env/monapi-ex5.inc
else
include $(MONADIR)/env/monapi-el5.inc
endif

include dependencies
