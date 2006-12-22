ifndef $(MONADIR)
MONADIR=$(PWD)/../../mona
endif

TARGET  = AUDIO
SOURCES = main.cpp ES1370Register.cpp ES1370Driver.cpp
INSTDIR = $(BINDIR)/APPS
INSTFILES = test.raw

SHAREDIR   = $(MONADIR)/share
ifneq ($(BUILD_TARGET),ELF)
#include $(SHAREDIR)/configs/monapi-ex5.inc
include ./libc-ex5.inc
else
include $(SHAREDIR)/configs/monapi-el5.inc
endif

ADDLINK = -lPci -lmonalibc

include dependencies
