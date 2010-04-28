TARGET  = MONADAT
SOURCES = \
	./Form1.cpp \
	./CodeConv.cpp \
	./SheetTab.cpp \
	./http.cpp
INSTDIR = $(BINDIR)/APPS/MONAFRMS/$(TARGET).APP
INSTFILES  = dat/MONALOGO.BM5 dat/02.DT5 dat/03.DT5 dat/04.DT5 dat/05.DT5 dat/06.DT5 dat/07.DT5 dat/08.DT5 dat/09.DT5 dat/10.DT5 dat/11.DT5
CLEANFILES = $(INSTFILES)

SHAREDIR   = $(MONADIR)/share
ifneq ($(BUILD_TARGET),ELF)
ADDLINK    = -lgui-imp -lmonalibc-imp --enable-auto-import
ADDLINKDEP = $(MONADIR)/lib/libgui-imp.a
include $(SHAREDIR)/configs/monapi-ex5.inc
else
ADDLINK    = -lgui -lmonalibc
ADDLINKDEP = $(MONADIR)/lib/libgui.a
include $(SHAREDIR)/configs/monapi-el5.inc
endif

CXXFLAGS += -DDEBUG

$(TARGET).EXE: $(INSTFILES)

.SUFFIXES: .dat .DT5
.dat.DT5:
	PATH=$(PATH):$(MONADIR)/tool/t5lzma bim2bin in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0

dat/MONALOGO.BM5: dat/MonaLogo.bmp
	bim2bin in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0

-include dependencies
