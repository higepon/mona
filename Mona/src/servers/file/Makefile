TARGET  = FILE.SVR
SOURCES = main.cpp file.cpp bzip2.cpp

MONADIR = ../../..
include $(MONADIR)/env/monapi-elf.inc

INSTDIR = $(BINDIR)/root/SERVERS
LINK    = -lbz2 -lmonapi -lmonalibc

$(TARGET): $(LIBDIR)/libbz2.a

include dependencies
