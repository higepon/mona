TARGET  = $(MONADIR)/bin/bim2bin$(EXEEXT)
SOURCES = bim2bin.c

MONADIR = ../..
include $(MONADIR)/env/Makefile.inc
CXXFLAGS = -Wall -g
INCLUDE  = -idirafter .

OBJECTS = $(SOURCES:.cpp =.o)

all : $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET)
	strip $(TARGET)

.SUFFIXES: .c .o
.c.o:
	$(CC) $(CXXFLAGS) $(INCLUDE) -c $<

depend:
	$(CC) -MM $(INCLUDE) $(CXXFLAGS) $(SOURCES) > dependencies

clean :
	rm -f bim2bin.o $(TARGET)

include dependencies
