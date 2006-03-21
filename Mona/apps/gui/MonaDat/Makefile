TARGET  = MONADAT
SOURCES = \
	./Form1.cpp \
	./CodeConv.cpp \
	./SheetTab.cpp

INSTDIR    = $(BINDIR)/iso/APPS/MONAFRMS/$(TARGET).APP
INSTFILES  = dat/MONALOGO.BM5 dat/02.DT5 dat/03.DT5 dat/04.DT5 dat/05.DT5 dat/06.DT5 dat/07.DT5 dat/08.DT5 dat/09.DT5 dat/10.DT5 dat/11.DT5
CLEANFILES = $(INSTFILES)

MONADIR = ../../..
ifneq ($(BUILD_TARGET),ELF)
ADDLINK    = -lgui-imp -lmonalibc-imp
ADDLINKDEP = $(MONADIR)/lib/libgui-imp.a
include $(MONADIR)/env/monapi-ex5.inc
else
ADDLINK    = -lgui -lmonalibc
ADDLINKDEP = $(MONADIR)/lib/libgui.a
include $(MONADIR)/env/monapi-el5.inc
endif

CXXFLAGS += -DDEBUG

$(TARGET).EXE: $(LIBDIR)/libgui.a $(INSTFILES)

#.SUFFIXES: .dat .DT2
#.dat.DT2:
#	bzip2 -c $< > $@
.SUFFIXES: .dat .DT5
.dat.DT5:
	$(MONADIR)/bin/bim2bin in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0

#dat/MONALOGO.BM2: dat/MonaLogo.bmp
#	bzip2 -c $< > $@
dat/MONALOGO.BM5: dat/MonaLogo.bmp
	$(MONADIR)/bin/bim2bin in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0

include dependencies
