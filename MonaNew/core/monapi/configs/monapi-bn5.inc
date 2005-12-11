include $(SHAREDIR)/configs/monapi.inc

.SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) -c -o $@ $(CXXFLAGS) $(INCLUDE) $<

.SUFFIXES: .c .o
.c.o:
	$(CC) -c -o $@ $(CFLAGS) $(INCLUDE) $<

.SUFFIXES: .asm .o
.asm.o:
	$(NASM) $(NFLAGS) -o $@ $<

all: $(TARGET).BN5


$(TARGET).BN5: $(TARGET).BIN
	bim2bin in:$< out:$@ -osacmp -tek5 BS:0 eprm:z0

$(TARGET).BIN: $(OBJECTS) $(LINKDEP)
	$(LD) --Map $(TARGET).map $(LFLAGS) -o $@ $(LIBDIR)/monapi.o $(OBJECTS) -L$(LIBDIR) $(LINK)
	$(STRIP) -O binary $@

install: all
	mkdir -p $(INSTDIR)
	$(INSTALL) $(TARGET).BN5 $(INSTFILES) $(INSTDIR)

clean:
	rm -f $(OBJECTS) $(TARGET).BIN $(TARGET).BN5 $(CLEANFILES) $(TARGET).map

depend dependencies:
ifneq ($(SOURCES),)
	$(CXX) -MM $(INCLUDE) $(CXXFLAGS) $(SOURCES) >> dependencies
endif
ifneq ($(CSOURCES),)
	$(CC) -MM $(INCLUDE) $(CFLAGS) $(SOURCES) >> dependencies
endif
ifneq ($(NASMSRCS),)
	for asm in $(NASMSRCS); do echo `echo $$asm | sed "s/\.asm/.o/`": "$$asm >> dependencies; done
endif
