TARGET  = libjpegls.a
SOURCES = jpegls.cpp

SHAREDIR = $(MONADIR)/share
include $(SHAREDIR)/configs/monapi.inc

INCLUDE = -I$(INCDIR)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	rm -f $@
	$(AR) $@ $(OBJECTS)
	$(RANLIB) $@

clean:
	rm -f $(OBJECTS) $(TARGET)

.SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) -c -o $(<:.cpp=.o) $(CXXFLAGS) $(INCLUDE) $<

depend:
	$(CXX) -MM $(INCLUDE) $(CXXFLAGS) $(SOURCES) > dependencies

install: $(TARGET)
	mkdir -p $(LIBDIR)
	$(INSTALL) -m 0644 $(TARGET) $(LIBDIR)/
	$(INSTALL) jpegls.h $(INCDIR)
include dependencies
