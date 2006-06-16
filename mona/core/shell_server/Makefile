SRCTOP=../..
include $(SRCTOP)/Makefile.inc

TARGET  = SHELL
SOURCES = main.cpp Shell.cpp InternalCommands.cpp
INSTDIR = $(BINDIR)/SERVERS

ADDLINK    = -lmonalibc
ADDLINKDEP = $(MONADIR)/lib/libmonalibc.a
SHAREDIR   = $(MONADIR)/share
ifneq ($(BUILD_TARGET),ELF)
include $(SHAREDIR)/configs/monapi-ex5.inc
else
include $(SHAREDIR)/configs/monapi-el5.inc
endif

-include dependencies
