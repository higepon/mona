TARGET  = BAYGUI
SOURCES = WindowManager.cpp
INSTDIR = $(BINDIR)/root/SERVERS
INSTFILES  = MONAWALL.BM5
CLEANFILES = MONAWALL.BM5

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

MONAWALL.BM5: MONAWALL.BMP
	$(MONADIR)/bin/bim2bin in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0

include dependencies
