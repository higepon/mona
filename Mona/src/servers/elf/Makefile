TARGET  = ELF.SVR
SOURCES = main.cpp elfparser.cpp

MONADIR = ../../..
include $(MONADIR)/env/app-elf.inc

INSTDIR = $(BINDIR)/root/SERVERS
LINK    = $(LIBDIR)/monapi.o -lmonapi -lmonalibc

include dependencies
