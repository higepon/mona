TARGET  = GUI
SOURCES = main.cpp image.cpp screen.cpp utils.cpp window.cpp effect.cpp Rectangle.cpp Overlap.cpp
INSTDIR = $(BINDIR)/root/SERVERS

MONADIR    = ../../..
ADDLINK    = -lmonalibc -ljpegls
ADDLINKDEP = $(MONADIR)/lib/libmonalibc.a $(MONADIR)/lib/libjpegls.a
ifneq ($(BUILD_TARGET),ELF)
include $(MONADIR)/env/monapi-ex2.inc
else
include $(MONADIR)/env/monapi-bn2.inc
endif

install-tinogui:
	$(INSTALL) STARTMF.MSH $(MONADIR)/bin/root/APPS
	$(INSTALL) BAYGUI.MSH $(MONADIR)/bin/root/APPS

install: install-tinogui

include dependencies
