LIB_STATIC   = libnicdrv.a
TARGET       = $(LIB_STATIC)

include $(MONADIR)/share/configs/Makefile.inc
include $(MONADIR)/share/configs/dirnames.inc

ifneq ($(BUILD_TARGET),ELF)
LIB_IMPORT   = libnicdrv-imp.a
LIB_DYNAMIC  = NICDRV.DLL
LIB_DYNAMIC5 = NICDRV.DL5
TARGET      += $(LIB_IMPORT) $(LIB_DYNAMIC) $(LIB_DYNAMIC5)
endif


SOURCES = AbstractMonic.cpp           \
          MonesFactory.cpp             \
          ne2000/Ne2000MoNic.cpp      \
          tulip/tulipMoNic.cpp

OBJECTS = $(SOURCES:.cpp=.o) $(NASMSRCS:.asm=.o)
INCLUDE = -I$(INCDIR) -I. -I..

all : $(TARGET)


$(LIB_STATIC): $(OBJECTS)
	rm -f $@
	$(AR) $@ $(OBJECTS)
	$(RANLIB) $@

ifneq ($(BUILD_TARGET),ELF)
$(LIB_IMPORT): $(LIB_DYNAMIC)
$(LIB_DYNAMIC): $(OBJECTS)
	$(LD) --export-all-symbols --out-implib $(LIB_IMPORT) -o $@ $(OBJECTS) -L$(LIBDIR) -lPci -lmonalibc -lmonapi-imp 
	$(STRIP) $@

$(LIB_DYNAMIC5): $(LIB_DYNAMIC)
#	bzip2 -c $(LIB_DYNAMIC) > $@
	bim2bin in:$(LIB_DYNAMIC) out:$@ -osacmp -tek5 BS:0 eprm:z0
endif

.SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCLUDE) -o $@ $<
	
.SUFFIXES: .asm .o
.asm.o:
	$(NASM) $(NFLAGS) -o $@ $<

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
ifneq ($(BUILD_TARGET),ELF)
	$(INSTALL) -p -m 0644 $(LIB_IMPORT) $(LIBDIR)/
	mkdir -p $(BINDIR)/LIBS
	$(INSTALL) -p $(LIB_DYNAMIC5) $(BINDIR)/LIBS/
	$(INSTALL) -p -m 0644 $(LIB_STATIC) $(LIBDIR)/
endif
	mkdir -p $(INCDIR)/nicdrv
	$(INSTALL) -p -m 0644 *.h $(INCDIR)/nicdrv/

uninstall:
	rm -f $(LIBDIR)/$(LIB_STATIC)
ifneq ($(BUILD_TARGET),ELF)
	rm -f $(LIBDIR)/$(LIB_IMPORT)
	rm -f $(BINDIR)/LIBS/$(LIB_DYNAMIC5)
endif

	for file in *.h; \
	do \
	  rm -f $(INCDIR)/nicdrv/$$file; \
	done
#	rmdir $(INCDIR)/nicdrv

include dependencies
