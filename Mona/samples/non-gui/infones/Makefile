TARGET  = INFONES
SOURCES = InfoNES.cpp InfoNES_System.cpp InfoNES_Mapper.cpp InfoNES_pAPU.cpp K6502.cpp
INSTDIR = $(BINDIR)/iso/APPS

MONADIR = ../../..
ifneq ($(BUILD_TARGET),ELF)
include $(MONADIR)/env/monapi-ex2.inc
else
include $(MONADIR)/env/monapi-el2.inc
endif

ADDLINK = -lmonalibc

include dependencies
