TARGET  = PE
SOURCES = main.cpp PEParser.cpp
INSTDIR = $(BINDIR)/iso/SERVERS

MONADIR    = ../../..
ADDLINK    = -lmonalibc
ADDLINKDEP = $(MONADIR)/lib/libmonalibc.a
include $(MONADIR)/env/monapi-bn5.inc

INCLUDE   += -I$(MONADIR)/include/stlport
include dependencies
