TARGET  = INFONES
SOURCES = InfoNES.cpp InfoNES_System_Mona.cpp InfoNES_Mapper.cpp InfoNES_pAPU.cpp K6502.cpp
INSTDIR = $(BINDIR)/APPS/MONAGUI

SHAREDIR   = $(MONADIR)/share
ifneq ($(BUILD_TARGET),ELF)
ADDLINK    = -lamonagui-imp -lmonalibc-imp --enable-auto-import
ADDLINKDEP = $(MONADIR)/lib/libamonagui-imp.a $(MONADIR)/lib/libmonalibc-imp.a
include $(SHAREDIR)/configs/monapi-ex5.inc
else
ADDLINK    = -lamonagui-imp -lmonalibc-imp -lmonapi-imp
ADDLINKDEP = $(MONADIR)/lib/libamonagui.a $(MONADIR)/lib/libmonalibc.a
include $(SHAREDIR)/configs/monapi-el5.inc
endif

CXXFLAGS += -DDEBUG

-include dependencies
