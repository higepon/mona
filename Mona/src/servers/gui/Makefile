TARGET  = GUI
SOURCES = main.cpp image.cpp screen.cpp utils.cpp window.cpp effect.cpp Rectangle.cpp Overlap.cpp
INSTDIR = $(BINDIR)/iso/SERVERS

MONADIR    = ../../..
ADDLINK    = -lmonalibc -ljpegls
ADDLINKDEP = $(MONADIR)/lib/libmonalibc.a $(MONADIR)/lib/libjpegls.a
ifneq ($(BUILD_TARGET),ELF)
include $(MONADIR)/env/monapi-ex5.inc
else
include $(MONADIR)/env/monapi-bn5.inc
endif

install-gui: MONAWALL.BM5
	$(INSTALL) STARTMF.MSH $(MONADIR)/bin/iso/APPS
	$(INSTALL) BAYGUI.MSH $(MONADIR)/bin/iso/APPS
#	$(INSTALL) MONAWALL.BM5 $(MONADIR)/bin/iso

#MONAWALL.BM5: MONAWALL.BMP
#	$(MONADIR)/bin/bim2bin in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0

install: install-gui

include dependencies
