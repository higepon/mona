include $(MONADIR)/env/monapi.inc

.SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) -c -o $@ $(CXXFLAGS) $(INCLUDE) $<

.SUFFIXES: .c .o
.c.o:
	$(CC) -c -o $@ $(CFLAGS) $(INCLUDE) $<

.SUFFIXES: .asm .o
.asm.o:
	$(NASM) $(NFLAGS) -o $@ -felf $<

all: $(TARGET).BN2


$(TARGET).BN2: $(TARGET).BIN
	bzip2 -c $< > $@

$(TARGET).BIN: $(OBJECTS) $(LIBMONAPI) $(LIBDIR)/libmonalibc.a
	$(LD) $(LFLAGS) $(LIBDIR)/monapi.o $(OBJECTS) -L$(LIBDIR) -o $@ $(LINK)
	$(STRIP)  -O binary $@

install: all
	mkdir -p $(INSTDIR)
	$(INSTALL) $(TARGET).BN2 $(INSTFILES) $(INSTDIR)

clean:
	rm -f $(OBJECTS) $(TARGET).BIN $(TARGET).BN2  $(CLEANFILES)

depend:
	rm -f dependencies
ifneq ($(SOURCES),)
	$(CXX) -MM $(INCLUDE) $(CXXFLAGS) $(SOURCES) >> dependencies
endif
ifneq ($(CSOURCES),)
	$(CC) -MM $(INCLUDE) $(CFLAGS) $(SOURCES) >> dependencies
endif
ifneq ($(NASMSRCS),)
	for asm in $(NASMSRCS); do echo `echo $$asm | sed "s/\.asm/.o/`": "$$asm >> dependencies; done
endif
