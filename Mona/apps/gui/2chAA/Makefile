TARGET  = 2CHAA.ELF
SOURCES = Form1.cpp

MONADIR = ../../..
include $(MONADIR)/env/app-elf.inc

INSTDIR = $(BINDIR)/root/APPS
LINK    = -lgui $(LIBDIR)/monapi.o -lmonapi -lmonalibc

$(TARGET): $(LIBDIR)/libgui.a

include dependencies
