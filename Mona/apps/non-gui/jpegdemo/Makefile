TARGET  = JPEGDEMO.ELF
SOURCES = jpegdemo.cpp

MONADIR = ../../..
include $(MONADIR)/env/app-elf.inc

INSTDIR = $(BINDIR)/root/APPS
LINK    = $(LIBDIR)/monapi.o -lmonapi -lmonalibc -ljpegls

$(TARGET): $(LIBDIR)/libjpegls.a

include dependencies
