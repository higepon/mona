LIBJDIR  = .
BINDIR   = $(MONADIR)/bin
LIBDIR   = $(MONADIR)/lib

ALLFLAGS = -Wall -O3 -g
CXXFLAGS = $(ALLFLAGS) -I$(MONADIR)/include -I$(LIBJDIR)
JFLAGS   = $(ALLFLAGS) $(CLSPATH)
CLSPATH  = --bootclasspath=$(LIBJDIR)
LDFLAGS  = -L$(MONADIR)/lib
LDWIN    = -mwindows
EXEEXT   = .exe

# options
## MinGW
#LDFLAGS  += -mno-cygwin
#CXXFLAGS += -I/usr/include/mingw -I/usr/include/w32api -DWIN32 -U__CYGWIN__
## speed up (danger!!)
#ALLFLAGS += -fno-bounds-check
#JFLAGS   += -fno-store-check

# options
## MONA
CXXFLAGS += -nostdlib -UWIN32 -U_WIN32 -fno-builtin -DMONA -DDEBUG_BUILD -DHOST_CYGWIN -fno-exceptions -fno-rtti

ifdef DLLNAME
TARGET    = JAVA.DLL
STATICLIB = $(LIBDIR)/lib$(DLLNAME).a
IMPORTLIB = $(LIBDIR)/lib$(DLLNAME)-imp.a
endif

CXXOBJS  = $(CXXSRCS:%.cpp=%.o)
JOBJECTS = $(JSOURCES:%.java=%.o)
JHEADERS = $(JSOURCES:%.java=%.h)
JCLASSES = $(JSOURCES:%.java=%.class)
OBJECTS  = $(CXXOBJS) $(JOBJECTS)

ifdef JAVAMAIN
OBJECTS += $(JAVAMAIN)-main.o
endif

all: $(TARGET)

$(JHEADERS): $(JCLASSES)

ifdef JAVAMAIN
$(JAVAMAIN)-main.o: $(JAVADIR)/javamain.cpp
	g++ $(CXXFLAGS) -o $@ -DJAVAMAIN=$(JAVAMAIN) -c $<
endif

.SUFFIXES: .cpp .o
.cpp.o:
	g++ $(CXXFLAGS) -o $@ -c $<

.SUFFIXES: .java .class
.java.class:
	gcj $(JFLAGS) -C $<

.SUFFIXES: .class .h
.class.h:
	gcjh $(CLSPATH) $*

.SUFFIXES: .java .o
.java.o:
	gcj $(JFLAGS) -o $@ -c $<

depend:
	rm -f dependencies
ifdef CXXSRCS
	for cpp in $(CXXSRCS); \
	do \
		echo -n `dirname $$cpp`/ >> dependencies; \
		g++ -MM $(CXXFLAGS) $$cpp >> dependencies; \
	done
endif
ifdef JSOURCES
	gcj -MM $(JFLAGS) $(JSOURCES) >> dependencies
endif

ifdef DLLNAME
$(TARGET):
	#g++ -shared -o $@ -Wl,--out-implib,$(IMPORTLIB) $(OBJECTS) $(LDFLAGS)
	rm -f $(STATICLIB)
	ar cvrus $(STATICLIB) $(OBJECTS)
	ranlib $(STATICLIB)
	ld --export-all-symbols --out-implib $(IMPORTLIB) -o $@ $(OBJECTS) $(LDFLAGS) -lsms_gc-imp -lmonapi-imp -lmonalibc-imp
	strip $@
	bim2bin in:$@ out:JAVA.DL5 -osacmp -tek5 BS:0 eprm:z0
	install -p JAVA.DL5 $(BINDIR)/LIBS

clean:
	rm -f $(TARGET) JAVA.DL5 $(BINDIR)/LIBS/JAVA.DL5 $(STATICLIB) $(IMPORTLIB) $(JHEADERS) $(JCLASSES) $(OBJECTS)
endif

-include dependencies
