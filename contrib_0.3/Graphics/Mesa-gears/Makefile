TARGET  = GEARS
SOURCES = gears.cpp
INSTDIR = $(BINDIR)/APPS


SHAREDIR   = $(MONADIR)/share
ifneq ($(BUILD_TARGET),ELF)
include $(SHAREDIR)/configs/monapi-exe.inc
else
include $(SHAREDIR)/configs/monapi-el5.inc
endif

ADDLINK  = -L../Mesa-6.2.1/lib -lOSMesa-imp -lmonalibc-imp
INCLUDE += -I../Mesa-6.2.1/include -I../tmpmonalibc

