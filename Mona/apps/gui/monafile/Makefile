TARGET  = MONAFILE
SOURCES = FileManager.cpp FileWindow.cpp FileBrowser.cpp Icon.cpp

INSTDIR    = $(BINDIR)/root/APPS/MONAFILE.APP
INSTFILES  = ICONS.BM2
CLEANFILES = $(INSTFILES)

MONADIR = ../../..
include $(MONADIR)/env/monapi-el2.inc

LINK    = -lgui -lmonapi -lmonalibc
CXXFLAGS += -DDEBUG

$(TARGET).ELF: $(LIBDIR)/libgui.a $(INSTFILES)

ICONS.BM2: icons.bmp
	bzip2 -c $< > $@

include dependencies
