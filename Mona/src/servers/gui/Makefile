TARGET  = GUI.BIN
SOURCES = main.cpp image.cpp screen.cpp utils.cpp window.cpp effect.cpp Rectangle.cpp Overlap.cpp

MONADIR = ../../..
include $(MONADIR)/env/monapi-bin.inc

INSTDIR = $(BINDIR)/root/SERVERS
LINK    = -lmonapi -lmonalibc -ljpegls

install-startgui:
	$(INSTALL) STARTGUI.MSH $(MONADIR)/bin/root/APPS

install: install-startgui

include dependencies
