TARGET    = REVERSI
SOURCES   = ReversiForm.cpp ReversiBoard.cpp Piece.cpp

INSTDIR   = $(BINDIR)/root/APPS/MONAFRMS/$(TARGET).APP
INSTFILES = RED.BMP WHITE.BMP

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

LINK      = -lgui -lmonapi -lmonalibc
CXXFLAGS += -DDEBUG

$(TARGET).EXE: $(LIBDIR)/libgui.a

include dependencies
