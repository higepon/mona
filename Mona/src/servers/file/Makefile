TARGET  = FILE
SOURCES = main.cpp file.cpp bzip2.cpp dtk5s.cpp FDCDriver.cpp fat.cpp fs.cpp IDEDriver.cpp ISO9660File.cpp ISO9660FileSystem.cpp ISO9660FileSystemManager.cpp FatFileSystemManager.cpp
INSTDIR = $(BINDIR)/iso/SERVERS

MONADIR    = ../../..
ADDLINK    = -lbz2 -lmonalibc
ADDLINKDEP = $(MONADIR)/lib/libbz2.a
include $(MONADIR)/env/monapi-bin.inc

INCLUDE   += -I$(TOOLSDIR)

include dependencies
