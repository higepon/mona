TARGET  = ELF
SOURCES = main.cpp elfparser.cpp
INSTDIR = $(BINDIR)/SERVERS

ADDLINK    = -lmonalibc
ADDLINKDEP = $(MONADIR)/lib/libmonalibc.a
SHAREDIR   = $(MONADIR)/share
include $(SHAREDIR)/configs/monapi-bn5.inc

distclean: clean
	rm dependencies
	touch dependencies

include dependencies
