# -*- Mode: Makefile ; Coding: utf-8 -*-
ifndef $(MONADIR)
export MONADIR=$(shell cd $(PWD)/../../../; pwd)
endif

TARGET  = TFILE
SOURCES = file.cpp
INSTDIR = $(BINDIR)/APPS/$(TARGET).APP
INSTFILES = this.is.my_long_file_name.file.txt this.is.my_unicode_file_name.あいう.file.txt TFILE.map
ADDCFLAGS = -DDEBUG -I . -I $(INCDIR)/monalibc -mno-stack-arg-probe -I $(MONADIR)/core/file_server

SHAREDIR   = $(MONADIR)/share
ADDLINK    =  ../../../core/file_server/VnodeManager.o ../../../core/file_server/VnodeCacher.o -lmonalibc-imp --enable-auto-import
ifneq ($(BUILD_TARGET),ELF)
include $(SHAREDIR)/configs/monapi-ex5.inc
else
include $(SHAREDIR)/configs/monapi-el5.inc
endif

-include dependencies
