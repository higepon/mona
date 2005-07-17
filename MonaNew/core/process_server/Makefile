TARGET  = PROCESS
SOURCES = main.cpp ProcessManager.cpp ProcessInfo.cpp
INSTDIR = $(BINDIR)

ADDLINK    = -lmonalibc
ADDLINKDEP = $(MONADIR)/lib/libmonalibc.a
SHAREDIR   = $(MONADIR)/share
include $(SHAREDIR)/configs/monapi-bin.inc

distclean: clean
	rm dependencies
	touch dependencies

include dependencies
