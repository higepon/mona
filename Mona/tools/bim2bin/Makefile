TARGET  = $(MONADIR)/bin/bim2bin$(EXEEXT)
SOURCES = bim2bin.c

MONADIR = ../..
include $(MONADIR)/env/Makefile.inc
CXXFLAGS = -Wall -g
CC = gcc
STRIP = strip
RM = -rm
INCLUDE  = -idirafter .

OBJECTS = $(SOURCES:.cpp =.o)

all : $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET)
	$(STRIP) $(TARGET)

.SUFFIXES: .c .o
.c.o:
	$(CC) $(CXXFLAGS) $(INCLUDE) -c $<

depend:
	$(CC) -MM $(INCLUDE) $(CXXFLAGS) $(SOURCES) > dependencies

clean :
	$(RM) -f bim2bin.o $(TARGET)

include dependencies
