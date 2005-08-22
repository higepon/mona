TARGET  = MONADAT
SOURCES = \
	./Form1.cpp \
	./CodeConv.cpp \
	./SheetTab.cpp

INSTDIR    = $(BINDIR)/iso/APPS/MONAFRMS/$(TARGET).APP
INSTFILES  = dat/MONALOGO.BM2 dat/02.DT2 dat/03.DT2 dat/04.DT2 dat/05.DT2 dat/06.DT2 dat/07.DT2 dat/08.DT2 dat/09.DT2 dat/10.DT2 dat/11.DT2
CLEANFILES = $(INSTFILES)

MONADIR = ../../..
ifneq ($(BUILD_TARGET),ELF)
ADDLINK    = -lgui-imp -lmonalibc-imp
ADDLINKDEP = $(MONADIR)/lib/libgui-imp.a
include $(MONADIR)/env/monapi-ex2.inc
else
ADDLINK    = -lgui -lmonalibc
ADDLINKDEP = $(MONADIR)/lib/libgui.a
include $(MONADIR)/env/monapi-el2.inc
endif

CXXFLAGS += -DDEBUG

$(TARGET).EXE: $(LIBDIR)/libgui.a $(INSTFILES)

.SUFFIXES: .dat .DT2
.dat.DT2:
	bzip2 -c $< > $@

dat/MONALOGO.BM2: dat/MonaLogo.bmp
	bzip2 -c $< > $@

include dependencies
