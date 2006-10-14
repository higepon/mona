DLLNAME  = java
CXXSRCS  = \
	javalang.cpp \
	java/lang/Object-CNI.cpp \
	java/lang/Class-CNI.cpp \
	java/lang/Math-CNI.cpp \
	java/io/PrintStream-CNI.cpp \
	SimpleCanvas-CNI.cpp
JBASESRC = \
	java/lang/Object.java \
	java/lang/Class.java
JSOURCES = \
	$(GCJSTRBUF) \
	java/lang/String.java \
	java/lang/StringBuffer.java \
	java/lang/System.java \
	java/lang/Math.java \
	java/io/PrintStream.java \
	java/util/ArrayList.java \
	SimpleCanvas.java
GCJSTRBUF = gnu/gcj/runtime/StringBuffer.java

include Makefile.inc

JCLASSES += $(JBASESRC:%.java=%.class)
OBJECTS  += $(JBASESRC:%.java=%.o) realize.o 
LDFLAGS  += -lsms_gc
$(TARGET): $(JHEADERS) $(OBJECTS)

$(GCJSTRBUF): java/lang/StringBuffer.java
	mkdir -p gnu/gcj/runtime
	echo "package gnu.gcj.runtime;" > $@
	echo -n "//" >> $@
	cat $< >> $@

realize.o: realize.cpp $(JHEADERS)
	g++ $(CXXFLAGS) -O0 -c $<

clean: clean2
clean2:
	rm -rf gnu
