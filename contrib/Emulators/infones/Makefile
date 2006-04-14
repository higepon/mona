TARGET  = INFONES
SOURCES = InfoNES.cpp InfoNES_System_Mona.cpp InfoNES_Mapper.cpp InfoNES_pAPU.cpp K6502.cpp
INSTDIR = $(BINDIR)/APPS

SHAREDIR   = $(MONADIR)/share
ifneq ($(BUILD_TARGET),ELF)
include $(SHAREDIR)/configs/monapi-ex5.inc
else
include $(SHAREDIR)/configs/monapi-el5.inc
endif

CXXFLAGS += -DDEBUG

include dependencies
