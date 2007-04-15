SRCTOP=../..
include $(SRCTOP)/Makefile.inc

# ifndef $(MONADIR)
# export MONADIR=$(shell cd $(PWD)/../../../mona; pwd)
# endif

TARGET  = TEST
SOURCES = test.cpp

INSTDIR = $(BINDIR)/APPS/TEST.APP
INSTFILES = TEST.TXT
$(TARGET).EX5: $(INSTFILES)
SHAREDIR   = $(MONADIR)/share
# link error
#ADDLINK    = -lmonalibc-imp

# link ok
ADDLINK    = -lmonalibc-imp
include $(SHAREDIR)/configs/monapi-ex5.inc

INCLUDE += -I $(INCDIR)/monalibc

-include dependencies
