TARGET  = $(LIBDIR)/libjpegls.a
SOURCES = jpegls.cpp

MONADIR = ../../..
include $(MONADIR)/env/monapi.inc
include $(MONADIR)/env/dirnames.inc

INCLUDE = -I$(INCDIR)

all : $(TARGET)

$(TARGET): $(OBJECTS)
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

include dependencies
