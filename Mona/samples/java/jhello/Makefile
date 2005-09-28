TARGET     = JHELLO
SOURCES    = main.cpp
JSOURCES   = A.java
INSTDIR    = $(BINDIR)/iso/APPS
CLEANFILES = $(OBJECTS) $(JCLASSES) *.class *.stackdump

MONADIR = ../../..
ifneq ($(BUILD_TARGET),ELF)
include $(MONADIR)/env/monapi-ex2.inc
else
include $(MONADIR)/env/monapi-el2.inc
endif

ADDLINK = -ljava -lsms_gc-imp -lmonalibc-imp
INCLUDE += -I$(MONADIR)/src/lib/java

COBJECTS = $(SOURCES:%.cpp=%.o)
JOBJECTS = $(JSOURCES:%.java=%.o)
JHEADERS = $(JSOURCES:%.java=%.h)
JCLASSES = $(JSOURCES:%.java=%.class)
JFLAGS   = --bootclasspath=$(MONADIR)/src/lib/java

all: $(TARGET)
$(COBJECTS): $(JHEADERS)
$(JHEADERS): $(JCLASSES)
$(JOBJECTS): $(JCLASSES)

.SUFFIXES: .java .class
.java.class:
	gcj $(JFLAGS) $(ALLFLAGS) -C $<

.SUFFIXES: .class .h
.class.h:
	gcjh $(JFLAGS) $*

.SUFFIXES: .java .o
.java.o:
	gcj $(JFLAGS) $(ALLFLAGS) -o $@ -c $<

include dependencies
