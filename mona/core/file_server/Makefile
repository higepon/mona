SRCTOP=../..
include $(SRCTOP)/Makefile.inc

TARGET  = FILE
SOURCES = main.cpp IDEDriver.cpp VnodeManager.cpp FileServer.cpp  VnodeCacher.cpp dtk5s.cpp vnode.cpp\
          ISO9660/ISO9660FileSystem.cpp FAT12/FAT12FileSystem.cpp FAT12/fat_write/fat.cpp FAT12/fat_write/endian_util.cpp FAT12/FDCDriver.cpp
INSTDIR = $(BINDIR)

ADDLINK    = -lmonalibc
ADDLINKDEP = $(MONADIR)/lib/libmonalibc.a
SHAREDIR   = $(MONADIR)/share
include $(SHAREDIR)/configs/monapi-bin.inc

INCLUDE += -I$(INCDIR)/monalibc -I$(INCDIR)/stlport -I$(INCDIR)/monapi -I . -I ./FAT12 -I ./ISO9660

-include dependencies
