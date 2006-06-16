SRCTOP=../..
include $(SRCTOP)/Makefile.inc

TARGET  = FILE
SOURCES = main.cpp file.cpp bzip2.cpp dtk5s.cpp FDCDriver.cpp fat.cpp fs.cpp IDEDriver.cpp ISO9660File.cpp ISO9660FileSystem.cpp ISO9660FileSystemManager.cpp FatFileSystemManager.cpp endian_util.cpp
INSTDIR = $(BINDIR)

ADDLINK    = -lmonalibc
ADDLINKDEP = $(MONADIR)/lib/libmonalibc.a
SHAREDIR   = $(MONADIR)/share
include $(SHAREDIR)/configs/monapi-bin.inc

-include dependencies
