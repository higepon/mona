TARGET  = GUI.SVR
SOURCES = main.cpp image.cpp screen.cpp utils.cpp bzip2.cpp

MONADIR = ../../..
include $(MONADIR)/env/app-elf.inc

INSTDIR = $(BINDIR)/root/SERVERS
LINK    = $(LIBDIR)/monapi.o -lbz2 -lmonapi -lmonalibc -ljpegls

$(TARGET): $(LIBDIR)/libbz2.a $(LIBDIR)/libjpegls.a

include dependencies
