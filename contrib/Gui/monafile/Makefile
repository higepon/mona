TARGET  = MONAFILE
SOURCES = FileManager.cpp FileWindow.cpp FileBrowser.cpp Icon.cpp
INSTDIR = $(BINDIR)/APPS/MONAFRMS/$(TARGET).APP
INSTFILES  = ICONS.BM5
CLEANFILES = $(INSTFILES)

SHAREDIR   = $(MONADIR)/share
ifneq ($(BUILD_TARGET),ELF)
ADDLINK    = -lgui-imp
ADDLINKDEP = $(MONADIR)/lib/libgui-imp.a
include $(SHAREDIR)/configs/monapi-ex5.inc
else
ADDLINK    = -lgui
ADDLINKDEP = $(MONADIR)/lib/libgui.a
include $(SHAREDIR)/configs/monapi-el5.inc
endif

CXXFLAGS += -DDEBUG

$(TARGET).EXE: $(LIBDIR)/libgui.a $(INSTFILES)

ICONS.BM5: icons.bmp
	bim2bin in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0

-include dependencies
