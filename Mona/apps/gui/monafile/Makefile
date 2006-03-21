TARGET  = MONAFILE
SOURCES = FileManager.cpp FileWindow.cpp FileBrowser.cpp Icon.cpp

INSTDIR    = $(BINDIR)/iso/APPS/MONAFRMS/$(TARGET).APP
INSTFILES  = ICONS.BM5
CLEANFILES = $(INSTFILES)

MONADIR = ../../..
ifneq ($(BUILD_TARGET),ELF)
ADDLINK    = -lgui-imp
ADDLINKDEP = $(MONADIR)/lib/libgui-imp.a
include $(MONADIR)/env/monapi-ex5.inc
else
ADDLINK    = -lgui
ADDLINKDEP = $(MONADIR)/lib/libgui.a
include $(MONADIR)/env/monapi-el5.inc
endif

CXXFLAGS += -DDEBUG

$(TARGET).EXE: $(LIBDIR)/libgui.a $(INSTFILES)

#ICONS.BM2: icons.bmp
#	bzip2 -c $< > $@
ICONS.BM5: icons.bmp
	$(MONADIR)/bin/bim2bin in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0

include dependencies
