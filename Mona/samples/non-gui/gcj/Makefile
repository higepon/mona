TARGET     = GCJ
SOURCES    = Class.cpp \
             javalang.cpp \
             main.cpp \
             Object.cpp \
             PrintStream.cpp \
             sms_gc/sms_gc.cpp \
             sms_gc/sms_ptr_dict.cpp
JBASESRC   = java/lang/Object.java \
             java/lang/Class.java
JSOURCES   = java/lang/String.java \
             java/lang/StringBuffer.java \
             java/lang/System.java \
             java/io/PrintStream.java \
             A.java \
             JNoiz2bg.java \
             SimpleCanvas.java
INSTDIR    = $(BINDIR)/iso/APPS
CLEANFILES = $(OBJECTS) $(JHEADERS) $(JCLASSES) *.class *.stackdump

MONADIR = ../../..
ifneq ($(BUILD_TARGET),ELF)
include $(MONADIR)/env/monapi-ex2.inc
else
include $(MONADIR)/env/monapi-el2.inc
endif

COBJECTS = $(SOURCES:%.cpp=%.o)
JOBJECTS = $(JSOURCES:%.java=%.o)
JHEADERS = $(JSOURCES:%.java=%.h)
JCLASSES = $(JBASESRC:%.java=%.class) $(JSOURCES:%.java=%.class)
OBJECTS  = realize.o $(COBJECTS) $(JBASESRC:%.java=%.o) $(JOBJECTS)
INCLUDE  = -I$(MONADIR)/include -I. -Isms_gc
JFLAGS   = --bootclasspath=.

all: $(TARGET)
$(COBJECTS): $(JHEADERS)
$(JHEADERS): $(JCLASSES)
$(JOBJECTS): $(JCLASSES)

#$(TARGET): $(OBJECTS)
#	g++ $(CFLAGS) -o $@ $(OBJECTS)

#.SUFFIXES: .cpp .o
#.cpp.o:
#	g++ $(CFLAGS) -o $@ -c $<

.SUFFIXES: .java .class
.java.class:
	gcj $(JFLAGS) $(ALLFLAGS) -C $<

.SUFFIXES: .class .h
.class.h:
	gcjh $(JFLAGS) $*

.SUFFIXES: .java .o
.java.o:
	gcj $(JFLAGS) $(ALLFLAGS) -o $@ -c $<
	g++ -c realize.cpp

include dependencies
