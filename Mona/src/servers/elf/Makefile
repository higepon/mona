TARGET  = ELF
SOURCES = main.cpp elfparser.cpp
INSTDIR = $(BINDIR)/root/SERVERS

MONADIR    = ../../..
ADDLINK    = -lmonalibc
ADDLINKDEP = $(MONADIR)/lib/libmonalibc.a
include $(MONADIR)/env/monapi-bn2.inc

include dependencies
