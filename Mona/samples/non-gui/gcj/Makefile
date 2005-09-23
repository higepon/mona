TARGET  = GCJ
SOURCES = main.cpp \
          javalang.cpp \
          PrintStream.cpp
JSRCS   = A.java \
          java/lang/Object.java \
          java/lang/Class.java \
          java/lang/System.java \
          java/io/PrintStream.java
INSTDIR = $(BINDIR)/iso/APPS

MONADIR = ../../..
ifneq ($(BUILD_TARGET),ELF)
include $(MONADIR)/env/monapi-ex2.inc
else
include $(MONADIR)/env/monapi-el2.inc
endif

INCLUDE = -I../../../include -I.
JFLAGS  = --bootclasspath=.
JCFLAGS = -g -I.

.SUFFIXES: .java .o
.java.o:
	gcj $(JFLAGS) $(JCFLAGS) -o $@ -c $<

.SUFFIXES: .java .class
.java.class:
	gcj $(JFLAGS) $(JCFLAGS) -C $<

main.o: A.h

A.h: A.class
	gcjh $(JFLAGS) A

A.class: A.java java/lang/Object.class java/lang/Class.class java/lang/System.class java/io/PrintStream.class
	gcj $(JFLAGS) $(JCFLAGS) -C $<

ADDLINK = -lmonalibc

include dependencies
