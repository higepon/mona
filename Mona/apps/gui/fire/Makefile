TARGET   = FIRE.ELF
SOURCES  = Form1.cpp
NASMSRCS = fire.asm

MONADIR = ../../..
include $(MONADIR)/env/app-elf.inc

INSTDIR = $(BINDIR)/root/APPS
LINK    = -lgui $(LIBDIR)/monapi.o -lmonapi -lmonalibc

$(TARGET): $(LIBDIR)/libgui.a

include dependencies
