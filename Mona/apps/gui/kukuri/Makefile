TARGET  = KUKURI
SOURCES = Kukuri.cpp

INSTDIR    = $(BINDIR)/root/APPS/KUKURI.APP
INSTFILES  = KUKURI.BM2
CLEANFILES = $(INSTFILES)

MONADIR = ../../..
include $(MONADIR)/env/monapi-el2.inc

LINK    = -lgui $(LIBDIR)/monapi.o -lmonapi -lmonalibc
CXXFLAGS += -DDEBUG

$(TARGET).ELF: $(LIBDIR)/libgui.a $(INSTFILES)

KUKURI.BM2: kukuri.bmp
	bzip2 -c $< > $@

include dependencies
