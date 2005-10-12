TARGET   = $(LIBDIR)/libjava.a
CSOURCES = \
	java_main.cpp \
	java_lang.cpp \
	java_lang_Class.cpp \
	java_lang_Math.cpp \
	java_lang_Object.cpp \
	java_io_PrintStream.cpp \
	java_util_Date.cpp \
	SimpleCanvas-Mona.cpp \
	org_monaos.cpp
JBASESRC = \
	java/lang/Object.java \
	java/lang/Class.java
JSOURCES = \
	java/lang/String.java \
	gnu/gcj/runtime/StringBuffer.java \
	java/lang/System.java \
	java/lang/Math.java \
	java/io/PrintStream.java \
	java/util/Date.java \
	SimpleCanvas.java \
	org/monaos/IO.java \
	org/monaos/MessageInfo.java \
	org/monaos/Message.java \
	org/monaos/Screen.java

MONADIR  = ../../..
include $(MONADIR)/env/monapi.inc
COBJECTS = $(CSOURCES:%.cpp=%.o)
JOBJECTS = $(JSOURCES:%.java=%.o)
JHEADERS = $(JSOURCES:%.java=%.h)
JCLASSES = $(JSOURCES:%.java=%.class)
OBJECTS  = $(COBJECTS) $(JOBJECTS)

all: $(TARGET)

$(COBJECTS): $(JHEADERS)
$(JHEADERS): $(JCLASSES)

.SUFFIXES: .cpp .o
.cpp.o:
	g++ $(CXXFLAGS) $(INCLUDE) -I. -o $@ -c $<

.SUFFIXES: .java .class
.java.class:
	gcj $(JFLAGS) -C $<

.SUFFIXES: .class .h
.class.h:
	gcjh $(CLSPATH) $*

.SUFFIXES: .java .o
.java.o:
	gcj $(JFLAGS) -o $@ -c $<

JCLASSES += $(JBASESRC:%.java=%.class)
OBJECTS  += $(JBASESRC:%.java=%.o) realize.o 

$(TARGET): $(OBJECTS)
	rm -f $@
	$(AR) $@ $(OBJECTS)
	$(RANLIB) $@

realize.o: realize.cpp $(JHEADERS)
	g++ -O0 -c $<

clean:
	rm -f $(TARGET) $(OBJECTS) $(JHEADERS) $(JCLASSES)

depend:
	$(CXX) -MM $(INCLUDE) -I. $(CXXFLAGS) $(CSOURCES) > dependencies

install: $(TARGET)

include dependencies
