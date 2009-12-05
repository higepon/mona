SRCTOP=../..
include $(SRCTOP)/Makefile.inc

TARGET     = NET
SOURCES    = main.cpp
CSOURCES   = sys_arch.c
INSTDIR    = $(BINDIR)/SERVERS
ADDLINK    = -lmonalibc-imp --enable-auto-import
ADDINCLUDE = -I lwip-1.3.1/src/include -I .

SHAREDIR   = $(MONADIR)/share
ifneq ($(BUILD_TARGET),ELF)
include $(SHAREDIR)/configs/monapi-ex5.inc
else
include $(SHAREDIR)/configs/monapi-el5.inc
endif

-include dependencies
