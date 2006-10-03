TARGET  = MONAPPT
SOURCES = monappt.cpp
INSTDIR = $(BINDIR)/APPS
INSTFILES  = MONAPPT.INI
#CLEANFILES = $(INSTFILES)

SHAREDIR   = $(MONADIR)/share
ifneq ($(BUILD_TARGET),ELF)
include $(SHAREDIR)/configs/monapi-ex5.inc
else
include $(SHAREDIR)/configs/monapi-el5.inc
endif

ADDLINK = -ljpegls

CXXFLAGS += -DDEBUG

$(TARGET).EXE: $(INSTFILES)

-include dependencies
