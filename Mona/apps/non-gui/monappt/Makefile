TARGET  = MONAPPT
SOURCES = monappt.cpp
INSTFILES  = MONAPPT.INI
INSTDIR = $(BINDIR)/iso/APPS/$(TARGET).APP

MONADIR = ../../..
ifneq ($(BUILD_TARGET),ELF)
include $(MONADIR)/env/monapi-ex5.inc
else
include $(MONADIR)/env/monapi-el5.inc
endif

ADDLINK    = -ljpegls
ADDLINKDEP = $(MONADIR)/lib/libjpegls.a

$(TARGET).EXE: $(INSTFILES)

include dependencies
