SRCTOP=../..
include $(SRCTOP)/Makefile.inc

TARGET  = CLIPBRD
SOURCES = main.cpp
INSTDIR = $(BINDIR)/SERVERS
INSTFILES = $(TARGET).map

ADDLINK    = -lmonalibc-imp --enable-auto-import

SHAREDIR   = $(MONADIR)/share
ifneq ($(BUILD_TARGET),ELF)
include $(SHAREDIR)/configs/monapi-ex5.inc
else
include $(SHAREDIR)/configs/monapi-el5.inc
endif

-include dependencies
