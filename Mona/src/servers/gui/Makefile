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

install-tinogui:
	$(INSTALL) STARTMF.MSH $(MONADIR)/bin/iso/APPS
	$(INSTALL) BAYGUI.MSH $(MONADIR)/bin/iso/APPS

install: install-tinogui

include dependencies
