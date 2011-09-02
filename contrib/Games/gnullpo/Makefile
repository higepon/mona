TARGET  = GNULLPO
SOURCES = gnullpo.cpp 

INSTDIR = $(BINDIR)/APPS/MONAGUI/GNULLPO.APP
INSTFILES = NULLPO.BM5 GAT.BM5
CLEANFILES = $(INSTFILES)
$(TARGET).EX5: $(INSTFILES)
SHAREDIR   = $(MONADIR)/share
ifneq ($(BUILD_TARGET),ELF)
ADDLINK    = -lamonagui-imp -lmonalibc-imp --enable-auto-import
ADDLINKDEP = $(MONADIR)/lib/libamonagui-imp.a
include $(SHAREDIR)/configs/monapi-ex5.inc
else
ADDLINK    = -lamonagui-imp -lmonalibc-imp -lmonapi-imp
ADDLINKDEP = $(MONADIR)/lib/libamonagui.a
include $(SHAREDIR)/configs/monapi-el5.inc
endif

CXXFLAGS += -DDEBUG
INCLUDE += -I .

NULLPO.BM5: nullpo.bmp
	`which bim2bin` in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0

GAT.BM5: gat.bmp
	`which bim2bin` in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0

-include dependencies
