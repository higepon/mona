TARGET  = FILE.SVR
SOURCES = main.cpp file.cpp bzip2.cpp

MONADIR = ../../..
include $(MONADIR)/env/app-elf.inc

INSTDIR = $(BINDIR)/root/SERVERS
LINK    = $(LIBDIR)/monapi.o -lbz2 -lmonapi -lmonalibc

$(TARGET): $(LIBDIR)/libbz2.a

include dependencies
