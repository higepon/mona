TARGET  = GNULLPO
SOURCES = gnullpo.cpp 

INSTDIR = $(BINDIR)/APPS/BAYGUI/GNULLPO.APP
INSTFILES = NULLPO.BM5 GAT.BM5
CLEANFILES = $(INSTFILES)
$(TARGET).EX5: $(INSTFILES)
SHAREDIR   = $(MONADIR)/share
ifneq ($(BUILD_TARGET),ELF)
ADDLINK    = -lbaygui-imp -lmonalibc-imp
ADDLINKDEP = $(MONADIR)/lib/libbaygui-imp.a
include $(SHAREDIR)/configs/monapi-ex5.inc
else
ADDLINK    = -lbaygui-imp -lmonalibc-imp -lmonapi-imp
ADDLINKDEP = $(MONADIR)/lib/libbaygui.a
include $(SHAREDIR)/configs/monapi-el5.inc
endif

CXXFLAGS += -DDEBUG
INCLUDE += -I .

NULLPO.BM5: nullpo.bmp
	bim2bin in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0

GAT.BM5: gat.bmp
	bim2bin in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0

-include dependencies
