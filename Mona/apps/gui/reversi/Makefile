TARGET    = REVERSI
SOURCES   = ReversiForm.cpp ReversiBoard.cpp Piece.cpp

INSTDIR   = $(BINDIR)/iso/APPS/MONAFRMS/$(TARGET).APP
INSTFILES = RED.BMP WHITE.BMP

MONADIR = ../../..
ifneq ($(BUILD_TARGET),ELF)
ADDLINK    = -lgui-imp -lmonalibc-imp
ADDLINKDEP = $(MONADIR)/lib/libgui-imp.a
include $(MONADIR)/env/monapi-ex5.inc
else
ADDLINK    = -lgui -lmonalibc
ADDLINKDEP = $(MONADIR)/lib/libgui.a
include $(MONADIR)/env/monapi-el5.inc
endif

CXXFLAGS += -DDEBUG

$(TARGET).EXE: $(LIBDIR)/libgui.a

include dependencies
