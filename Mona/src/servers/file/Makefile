TARGET  = FILE
SOURCES = main.cpp file.cpp bzip2.cpp dtk5s.cpp IDEDriver.cpp ISO9660.cpp FDCDriver.cpp fat.cpp fs.cpp
INSTDIR = $(BINDIR)/root/SERVERS

MONADIR    = ../../..
ADDLINK    = -lbz2 -lmonalibc
ADDLINKDEP = $(MONADIR)/lib/libbz2.a
include $(MONADIR)/env/monapi-bin.inc

INCLUDE   += -I$(TOOLSDIR)

include dependencies
