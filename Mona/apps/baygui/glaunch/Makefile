TARGET  = GLAUNCH
SOURCES = FileManager.cpp FileWindow.cpp FileBrowser.cpp Icon.cpp
INSTDIR = $(BINDIR)/root/APPS/GLAUNCH.APP
INSTFILES = ICONS.BM5
CLEANFILES = $(INSTFILES)

$(TARGET).EL5: $(INSTFILES)

MONADIR    = ../../..
ADDLINK    = -lgui
ADDLINKDEP = $(MONADIR)/lib/libgui.a
include $(MONADIR)/env/monapi-el5.inc

CXXFLAGS += -DDEBUG

ICONS.BM5: icons.bmp
	$(MONADIR)/bin/bim2bin in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0

include dependencies
