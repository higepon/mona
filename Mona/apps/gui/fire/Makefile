TARGET   = FIRE
SOURCES  = Form1.cpp
NASMSRCS = fire.asm

MONADIR = ../../..
include $(MONADIR)/env/monapi-el2.inc

INSTDIR = $(BINDIR)/root/APPS
LINK    = -lgui $(LIBDIR)/monapi.o -lmonapi -lmonalibc
CXXFLAGS += -DDEBUG

$(TARGET).ELF: $(LIBDIR)/libgui.a

include dependencies
