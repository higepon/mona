TARGET     = GCJ
SOURCES    = main.cpp \
             javalang.cpp \
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
             A.java
INSTDIR    = $(BINDIR)/iso/APPS
CLEANFILES = A.h $(OBJECTS) $(JHEADERS) $(JCLASSES) *.stackdump

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
OBJECTS  = $(COBJECTS) $(JBASESRC:%.java=%.o) $(JOBJECTS)
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

.SUFFIXES: .class .o
.class.o:
	gcj $(JFLAGS) $(ALLFLAGS) -o $@ -c $<

include dependencies
