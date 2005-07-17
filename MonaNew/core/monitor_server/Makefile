TARGET  = MONITOR
SOURCES = main.cpp
INSTDIR = $(BINDIR)

SHAREDIR   = $(MONADIR)/share
include $(SHAREDIR)/configs/monapi-bin.inc

distclean: clean
	rm dependencies
	touch dependencies

include dependencies
