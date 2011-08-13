# -*- Mode: Makefile ; Coding: utf-8 -*-
ifndef $(MONADIR)
export MONADIR=$(shell cd $(PWD)/../../../; pwd)
endif

TARGET  = TFLOAD
SOURCES = file_load.cpp
INSTDIR = $(BINDIR)/APPS/$(TARGET).APP
INSTFILES = TFLOAD.map
ADDCFLAGS = -DDEBUG -I .  -mno-stack-arg-probe -I $(MONADIR)/core/file_server

SHAREDIR   = $(MONADIR)/share
ADDLINK    =  -lmonalibc-imp --enable-auto-import
ifneq ($(BUILD_TARGET),ELF)
include $(SHAREDIR)/configs/monapi-ex5.inc
else
include $(SHAREDIR)/configs/monapi-el5.inc
endif

-include dependencies
