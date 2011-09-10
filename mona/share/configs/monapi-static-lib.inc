include $(MONADIR)/share/configs/monapi.inc

OBJECTS = $(patsubst %.cpp, %.o, $(SOURCES))
CXXFLAGS+=$(ADDCXXFLAGS)

all : $(DEP_TARGET) $(TARGET)

$(LIB_STATIC): $(OBJECTS)
	rm -f $@
	$(AR) $@ $(OBJECTS)
	$(RANLIB) $@

.SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCLUDE) $< -o $@

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
