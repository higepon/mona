TARGET  = KUKURI
SOURCES = Kukuri.cpp

INSTDIR    = $(BINDIR)/root/APPS/MONAFRMS/KUKURI.APP
INSTFILES  = KUKURI.BM2
CLEANFILES = $(INSTFILES)

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

LINK    = -lgui -lmonapi -lmonalibc
CXXFLAGS += -DDEBUG

$(TARGET).EXE: $(LIBDIR)/libgui.a $(INSTFILES)

KUKURI.BM2: kukuri.bmp
	bzip2 -c $< > $@

include dependencies
