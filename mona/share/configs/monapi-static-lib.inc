include $(MONADIR)/share/configs/monapi.inc

CPPOBJECTS = $(SOURCES:.cpp=.o)
CPPOBJECTS2 = $(patsubst %.cc,%.o, $(CPPOBJECTS))
OBJECTS   = $(CPPOBJECTS2) $(CSOURCES:.c=.o) $(NASMSRCS:.asm=.o) $(JSOURCES:.java=.o)
CXXFLAGS+=$(ADDCXXFLAGS)

all : $(DEP_TARGET) $(TARGET)

$(LIB_STATIC): $(OBJECTS)
	rm -f $@
	$(AR) $@ $(OBJECTS)
	$(RANLIB) $@

.SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCLUDE) $< -o $@

.SUFFIXES: .cc .o
.cc.o:
	$(CXX) -c -o $@ $(CXXFLAGS) $(INCLUDE) $<

depend:
	$(CXX) -MM $(INCLUDE) $(CXXFLAGS) $(SOURCES) > dependencies

clean:
	rm -f $(TARGET) $(OBJECTS)

distclean: clean
	rm dependencies
	touch dependencies

install: $(TARGET)
	mkdir -p $(LIBDIR)
	$(INSTALL) -p -m 0644 $(LIB_STATIC) $(LIBDIR)/
#	mkdir -p $(INCDIR)/pci
#	$(INSTALL) -p -m 0644 pci/*.h $(INCDIR)/pci/

uninstall:
	rm -f $(LIBDIR)/$(LIB_STATIC)

check-syntax:
include $(MONADIR)/share/configs/monapi.inc

CPPOBJECTS = $(SOURCES:.cpp=.o)
CPPOBJECTS2 = $(patsubst %.cc,%.o, $(CPPOBJECTS))
OBJECTS   = $(CPPOBJECTS2) $(CSOURCES:.c=.o) $(NASMSRCS:.asm=.o) $(JSOURCES:.java=.o)
CXXFLAGS+=$(ADDCXXFLAGS)

all : $(DEP_TARGET) $(TARGET)

$(LIB_STATIC): $(OBJECTS)
	rm -f $@
	$(AR) $@ $(OBJECTS)
	$(RANLIB) $@

.SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCLUDE) $< -o $@

.SUFFIXES: .cc .o
.cc.o:
	$(CXX) -c -o $@ $(CXXFLAGS) $(INCLUDE) $<

depend:
	$(CXX) -MM $(INCLUDE) $(CXXFLAGS) $(SOURCES) > dependencies

clean:
	rm -f $(TARGET) $(OBJECTS)

distclean: clean
	rm dependencies
	touch dependencies

install: $(TARGET)
	mkdir -p $(LIBDIR)
	$(INSTALL) -p -m 0644 $(LIB_STATIC) $(LIBDIR)/
#	mkdir -p $(INCDIR)/pci
#	$(INSTALL) -p -m 0644 pci/*.h $(INCDIR)/pci/

uninstall:
	rm -f $(LIBDIR)/$(LIB_STATIC)

check-syntax:
	echo "hora"
	echo $(FOO)
	echo ${CHK_SOURCES}
	LANG=C $(CXX) -o nul -fsyntax-only $(CXXFLAGS) $(INCLUDE) -S $(FLYMAKE_DIR)${CHK_SOURCES}
