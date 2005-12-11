LIB_STATIC   = libmonalibc.a
TARGET       = $(LIB_STATIC)

include $(MONADIR)/share/configs/Makefile.inc
include $(MONADIR)/share/configs/dirnames.inc

ifneq ($(BUILD_TARGET),ELF)
LIB_IMPORT   = libmonalibc-imp.a
LIB_DYNAMIC  = MONALIBC.DLL
LIB_DYNAMIC5 = MONALIBC.DL5
TARGET      += $(LIB_IMPORT) $(LIB_DYNAMIC) $(LIB_DYNAMIC5)
endif

SOURCES = ctype.cpp    \
          monalibc.cpp \
          stdarg.cpp   \
          stdio.cpp    \
          stdlib.cpp   \
          math.cpp     \

OBJECTS = $(SOURCES:.cpp=.o)
INCLUDE = -I$(INCDIR) -I. -I..

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

$(LIB_DYNAMIC5): $(LIB_DYNAMIC)
#	bzip2 -c $(LIB_DYNAMIC) > $@
	bim2bin in:$(LIB_DYNAMIC) out:$@ -osacmp -tek5 BS:0 eprm:z0
endif

.SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCLUDE) $<

depend dependencies:
	$(CXX) -MM $(INCLUDE) $(CXXFLAGS) $(SOURCES) > dependencies

clean:
	rm -f $(TARGET) $(OBJECTS) dependencies

install: $(TARGET)
	mkdir -p $(LIBDIR)
	$(INSTALL) -m 0644 $(LIB_STATIC) $(LIBDIR)/
ifneq ($(BUILD_TARGET),ELF)
	$(INSTALL) -m 0644 $(LIB_IMPORT) $(LIBDIR)/
	mkdir -p $(BINDIR)/LIBS
	$(INSTALL) $(LIB_DYNAMIC5) $(BINDIR)/LIBS/
endif
	mkdir -p $(INCDIR)/monalibc
	$(INSTALL) -m 0644 *.h $(INCDIR)/monalibc/
	mv $(INCDIR)/monalibc/monalibc.h $(INCDIR)/

uninstall:
	rm -f $(LIBDIR)/$(LIB_STATIC)
ifneq ($(BUILD_TARGET),ELF)
	rm -f $(LIBDIR)/$(LIB_IMPORT)
	rm -f $(BINDIR)/LIBS/$(LIB_DYNAMIC5)
endif
	rm -f $(INCDIR)/monalibc.h
	for file in *.h; \
	do \
	  rm -f $(INCDIR)/monalibc/$$file; \
	done

-include dependencies
