TARGET  = LABEL
SOURCES = Form1.cpp

MONADIR = ../../..
include $(MONADIR)/env/monapi-el2.inc

INSTDIR = $(BINDIR)/root/APPS
LINK    = -lgui $(LIBDIR)/monapi.o -lmonapi -lmonalibc
CXXFLAGS += -DDEBUG

$(TARGET).ELF: $(LIBDIR)/libgui.a

include dependencies
