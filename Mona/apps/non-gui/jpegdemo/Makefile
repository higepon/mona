TARGET  = JPEGDEMO
SOURCES = jpegdemo.cpp
INSTDIR = $(BINDIR)/iso/APPS

MONADIR    = ../../..
ADDLINK    = -ljpegls
ADDLINKDEP = $(MONADIR)/lib/libjpegls.a
ifneq ($(BUILD_TARGET),ELF)
include $(MONADIR)/env/monapi-ex2.inc
else
include $(MONADIR)/env/monapi-el2.inc
endif

include dependencies
