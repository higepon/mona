TARGET  = GNULLPO
SOURCES = gnullpo.cpp 
INSTDIR = $(BINDIR)/root/APPS/GNULLPO.APP
INSTFILES = NULLPO.BM5 GAT.BM5
CLEANFILES = $(INSTFILES)

$(TARGET).EX5: $(INSTFILES)

MONADIR    = ../../..
#ifneq ($(BUILD_TARGET),ELF)
#ADDLINK    = -lbaygui-imp
#ADDLINKDEP = $(MONADIR)/lib/libbaygui-imp.a
#include $(MONADIR)/env/monapi-ex2.inc
#else
ADDLINK    = -lbaygui
ADDLINKDEP = $(MONADIR)/lib/libbaygui.a
include $(MONADIR)/env/monapi-el2.inc
#endif

CXXFLAGS += -DDEBUG

NULLPO.BM5: nullpo.bmp
	$(MONADIR)/bin/bim2bin in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0

GAT.BM5: gat.bmp
	$(MONADIR)/bin/bim2bin in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0

include dependencies
