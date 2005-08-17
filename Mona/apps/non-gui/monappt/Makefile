TARGET  = MONAPPT
SOURCES = monappt.cpp
INSTFILES  = MONAPPT.INI
INSTDIR = $(BINDIR)/iso/APPS/$(TARGET).APP

MONADIR = ../../..
ifneq ($(BUILD_TARGET),ELF)
include $(MONADIR)/env/monapi-ex2.inc
else
include $(MONADIR)/env/monapi-el2.inc
endif

$(TARGET).EXE: $(INSTFILES)

include dependencies
