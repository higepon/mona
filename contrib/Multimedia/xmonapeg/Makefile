TARGET  = XMONAPEG
SOURCES = monapeg.cpp
INSTDIR = $(BINDIR)/APPS/MONAGUI

#INSTFILES = SAMPLE.MPG

SHAREDIR   = $(MONADIR)/share
ifneq ($(BUILD_TARGET),ELF)
include $(SHAREDIR)/configs/monapi-ex5.inc
else
include $(SHAREDIR)/configs/monapi-el5.inc
endif

ADDLINK = -L. -llen-cygwin -lamonagui-imp --enable-auto-import

-include dependencies
