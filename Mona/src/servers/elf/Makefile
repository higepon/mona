TARGET  = ELF.SVR
SOURCES = main.cpp elfparser.cpp ProcessManager.cpp ProcessInfo.cpp

MONADIR = ../../..
include $(MONADIR)/env/monapi-elf.inc

INSTDIR = $(BINDIR)/root/SERVERS
LINK    = $(LIBDIR)/monapi.o -lmonapi -lmonalibc

include dependencies
