TARGET    = GWABA
SOURCES   = wabamona.cpp

INSTDIR   = $(BINDIR)/root/APPS
INSTFILES = WABA.LIB HELLO.WRP

MONADIR = ../../..
include $(MONADIR)/env/monapi-el2.inc

LINK    = -lgui -lmonapi -lmonalibc
CXXFLAGS += -DDEBUG

$(TARGET).ELF: $(LIBDIR)/libgui.a

include dependencies
