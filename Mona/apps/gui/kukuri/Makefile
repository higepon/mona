TARGET  = KUKURI
SOURCES = Kukuri.cpp

INSTDIR    = $(BINDIR)/iso/APPS/MONAFRMS/$(TARGET).APP
INSTFILES  = KUKURI.BM5
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

#KUKURI.BM2: kukuri.bmp
#	bzip2 -c $< > $@
KUKURI.BM5: kukuri.bmp
	$(MONADIR)/bin/bim2bin in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0

include dependencies
