TARGET  = GUI.SVR
SOURCES = main.cpp image.cpp screen.cpp utils.cpp

MONADIR = ../../..
include $(MONADIR)/env/monapi-elf.inc

INSTDIR = $(BINDIR)/root/SERVERS
LINK    = $(LIBDIR)/monapi.o -lmonapi -lmonalibc -ljpegls

$(TARGET): $(LIBDIR)/libjpegls.a

include dependencies
