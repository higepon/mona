LIB_STATIC   = $(LIBDIR)/libmonalibc.a
TARGET       = $(LIB_STATIC)

ifneq ($(BUILD_TARGET),ELF)
LIB_IMPORT   = $(LIBDIR)/libmonalibc-imp.a
LIB_DYNAMIC  = MONALIBC.DLL
LIB_DYNAMIC2 = MONALIBC.DL2
TARGET      += $(LIB_IMPORT) $(LIB_DYNAMIC) $(LIB_DYNAMIC2)
endif

SOURCES = ctype.cpp    \
          monalibc.cpp \
          stdarg.cpp   \
          stdio.cpp    \
          stdlib.cpp   \
          math.cpp     \

MONADIR  = ../../..
include $(MONADIR)/env/dirnames.inc
include $(MONADIR)/env/Makefile.inc

INCLUDE = -I$(INCDIR)
OBJECTS = $(SOURCES:.cpp=.o)

all : $(TARGET)

$(LIB_STATIC): $(OBJECTS)
	rm -f $@
	$(AR) $@ $(OBJECTS)
	$(RANLIB) $@

ifneq ($(BUILD_TARGET),ELF)
$(LIB_IMPORT): $(LIB_DYNAMIC)
$(LIB_DYNAMIC): $(OBJECTS)
	$(LD) --export-all-symbols --out-implib $(LIB_IMPORT) -o $@ $(OBJECTS) -L$(MONADIR)/lib -lmonapi-imp
	$(STRIP) $@

$(LIB_DYNAMIC2): $(LIB_DYNAMIC)
	bzip2 -c $(LIB_DYNAMIC) > $@
endif

.SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCLUDE) $<

clean :
	rm -f $(TARGET) $(OBJECTS)

depend:
	$(CXX) -MM $(INCLUDE) $(CXXFLAGS) $(SOURCES) > dependencies

install: $(TARGET)
ifneq ($(BUILD_TARGET),ELF)
	mkdir -p $(MONADIR)/bin/root/LIBS
	$(INSTALL) $(LIB_DYNAMIC2) $(MONADIR)/bin/root/LIBS
endif

include dependencies
