LIB_STATIC  = $(LIBDIR)/libgui.a
TARGET      = $(LIB_STATIC)

ifeq ($(BUILD_TARGET),PE-DLL)
LIB_IMPORT  = $(LIBDIR)/libgui-imp.a
LIB_DYNAMIC = GUI.DLL
TARGET     += $(LIB_STATIC) $(LIB_IMPORT) $(LIB_DYNAMIC)
endif

SOURCES = \
	./stub.cpp \
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
	System/Mona/Forms/Cursor.cpp \
	System/Mona/Forms/Form.cpp \
	System/Mona/Forms/Label.cpp \
	System/Mona/Forms/Timer.cpp \
	System/Text/Encoding.cpp

MONADIR = ../../..
include $(MONADIR)/env/monapi.inc

CXXFLAGS += -DDEBUG

INCLUDE = -I$(INCDIR)

all: $(TARGET)

$(LIB_STATIC): $(OBJECTS)
	rm -f $@
	$(AR) $@ $(OBJECTS)
	$(RANLIB) $@

ifeq ($(BUILD_TARGET),PE-DLL)
$(LIB_IMPORT): $(LIB_DYNAMIC)
$(LIB_DYNAMIC): $(OBJECTS)
	dllwrap -e dllmain --export-all --implib $(LIB_IMPORT) -o $@ $(OBJECTS) -L$(MONADIR)/lib -lmonapi-imp
	$(STRIP) $@
endif

.SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) -c -o $(<:.cpp=.o) $(CXXFLAGS) $(INCLUDE) $<

clean:
	rm -f $(OBJECTS) $(TARGET)

depend:
	rm -f dependencies
	for src in $(SOURCES); \
	do \
		echo -n `echo $$src | sed "s/\(.*\)\/.*$$/\1\//"` >> dependencies; \
		$(CXX) -MM $(INCLUDE) $(CXXFLAGS) $$src >> dependencies; \
	done

install: $(TARGET)
ifeq ($(BUILD_TARGET),PE-DLL)
	mkdir -p $(MONADIR)/bin/root/LIBS
	$(INSTALL) $(LIB_DYNAMIC) $(MONADIR)/bin/root/LIBS
endif

include dependencies
