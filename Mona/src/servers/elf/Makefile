TARGET  = ELF
SOURCES = main.cpp elfparser.cpp
INSTDIR = $(BINDIR)/iso/SERVERS

MONADIR    = ../../..
ADDLINK    = -lmonalibc
ADDLINKDEP = $(MONADIR)/lib/libmonalibc.a
include $(MONADIR)/env/monapi-bn5.inc

include dependencies
