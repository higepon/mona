TARGET    = GREVERSI
SOURCES   = ReversiForm.cpp ReversiBoard.cpp Piece.cpp

INSTDIR   = $(BINDIR)/root/APPS/GREVERSI.APP
INSTFILES = RED.BMP WHITE.BMP

MONADIR = ../../..
include $(MONADIR)/env/monapi-el2.inc

LINK      = -lgui $(LIBDIR)/monapi.o -lmonapi -lmonalibc
CXXFLAGS += -DDEBUG

$(TARGET): $(LIBDIR)/libgui.a

include dependencies
