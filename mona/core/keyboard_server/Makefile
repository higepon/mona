SRCTOP=../..
include $(SRCTOP)/Makefile.inc

TARGET  = KEYBDMNG
SOURCES = main.cpp KeyboardManager.cpp
INSTDIR = $(BINDIR)/SERVERS

ADDLINK    = -lmonalibc-imp

SHAREDIR   = $(MONADIR)/share
ifneq ($(BUILD_TARGET),ELF)
include $(SHAREDIR)/configs/monapi-ex5.inc
else
include $(SHAREDIR)/configs/monapi-el5.inc
endif

-include dependencies
