TARGET  = JPEGDEMO
SOURCES = jpegdemo.cpp
INSTDIR = $(BINDIR)/APPS

SHAREDIR   = $(MONADIR)/share
ifneq ($(BUILD_TARGET),ELF)
include $(SHAREDIR)/configs/monapi-ex5.inc
else
include $(SHAREDIR)/configs/monapi-el5.inc
endif

ADDLINK    = -ljpegls
ADDLINKDEP = $(MONADIR)/lib/libjpegls.a

include dependencies
