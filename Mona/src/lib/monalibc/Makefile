SOURCES = ctype.cpp    \
          monalibc.cpp \
          stdarg.cpp   \
          stdio.cpp    \
          stdlib.cpp

MONADIR  = ../../..
include $(MONADIR)/env/dirnames.inc
include $(MONADIR)/env/Makefile.inc

TARGET  = $(LIBDIR)/libmonalibc.a

INCLUDE = -I$(INCDIR)
OBJECTS = $(SOURCES:.cpp=.o)

all : $(TARGET)

$(TARGET) : $(OBJECTS)
	$(AR) $@ $(OBJECTS)
	$(RANLIB) $@

.SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCLUDE) $<

clean :
	rm -f $(TARGET) $(OBJECTS)

depend:
	$(CXX) -MM $(INCLUDE) $(CXXFLAGS) $(SOURCES) > dependencies

include dependencies
