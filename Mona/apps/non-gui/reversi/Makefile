TARGET    = REVERSI
SOURCES   = ReversiBoard.cpp reversi.cpp

INSTDIR   = $(BINDIR)/root/APPS

MONADIR = ../../..
ifneq ($(BUILD_TARGET),ELF)
ADDLINK    = -lgui-imp
ADDLINKDEP = $(MONADIR)/lib/libgui-imp.a
include $(MONADIR)/env/monapi-ex2.inc
else
ADDLINK    = -lgui
ADDLINKDEP = $(MONADIR)/lib/libgui.a
include $(MONADIR)/env/monapi-el2.inc
endif

CXXFLAGS += -DDEBUG

$(TARGET).EXE: $(LIBDIR)/libgui.a

include dependencies
