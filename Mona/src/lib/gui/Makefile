SOURCES = \
	System/Console.cpp \
	System/String.cpp \
	System/Drawing/Bitmap.cpp \
	System/Drawing/Font.cpp \
	System/Drawing/Graphics.cpp \
	System/Drawing/Rectangle.cpp \
	System/Mona/Forms/Application.cpp \
	System/Mona/Forms/Button.cpp \
	System/Mona/Forms/Control.cpp \
	System/Mona/Forms/ControlPaint.cpp \
	System/Mona/Forms/Form.cpp \
	System/Mona/Forms/Label.cpp \
	System/Text/Encoding.cpp

MONADIR = ../../..
include $(MONADIR)/env/monapi.inc
include $(MONADIR)/env/dirnames.inc

TARGET  = $(LIBDIR)/libgui.a

CXXFLAGS += -DDEBUG -DMONA

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

include dependencies
