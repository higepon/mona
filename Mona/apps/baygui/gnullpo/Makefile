TARGET  = GNULLPO
SOURCES = gnullpo.cpp 
INSTDIR = $(BINDIR)/root/APPS/GNULLPO.APP
INSTFILES = HENORMAL.BM5 HEPUSHED.BM5

$(TARGET).EX5: $(INSTFILES)

MONADIR    = ../../..
ifneq ($(BUILD_TARGET),ELF)
ADDLINK    = -lbaygui-imp
ADDLINKDEP = $(MONADIR)/lib/libbaygui-imp.a
include $(MONADIR)/env/monapi-ex5.inc
else
ADDLINK    = -lbaygui
ADDLINKDEP = $(MONADIR)/lib/libbaygui.a
include $(MONADIR)/env/monapi-el5.inc
endif

CXXFLAGS += -DDEBUG

HENORMAL.BM5: henormal.bmp
	$(MONADIR)/bin/bim2bin in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0

HEPUSHED.BM5: hepushed.bmp
	$(MONADIR)/bin/bim2bin in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0

include dependencies
