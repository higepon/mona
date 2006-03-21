TARGET  = INFONES
SOURCES = InfoNES.cpp InfoNES_System_Mona.cpp InfoNES_Mapper.cpp InfoNES_pAPU.cpp K6502.cpp
INSTDIR = $(BINDIR)/iso/APPS

MONADIR = ../../..
ifneq ($(BUILD_TARGET),ELF)
include $(MONADIR)/env/monapi-ex5.inc
else
include $(MONADIR)/env/monapi-el5.inc
endif

ADDLINK = -lmonalibc

include dependencies
