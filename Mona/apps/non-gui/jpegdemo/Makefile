TARGET  = JPEGDEMO
SOURCES = jpegdemo.cpp

MONADIR = ../../..
include $(MONADIR)/env/monapi-el2.inc

INSTDIR = $(BINDIR)/root/APPS
LINK    = -lmonapi -lmonalibc -ljpegls

$(TARGET).ELF: $(LIBDIR)/libjpegls.a

include dependencies
