TARGET  = MONA-12.h MONA-12.MNF
BIT2MNF = bit2mnf$(EXEEXT)
FONTDIR = ../monafont

MONADIR = ../..
include $(MONADIR)/env/Makefile.inc
CXXFLAGS = -Wall

all: $(TARGET)

$(TARGET): $(BIT2MNF)
	./$(BIT2MNF) -o MONA-12 \
		--ascii $(FONTDIR)/mona6x12.bit0 \
		--ascii $(FONTDIR)/mona6x12a.bit1 \
		--jisx0201 $(FONTDIR)/mona6x12r.bit1 \
		--jisx0208 $(FONTDIR)/monak12-base.bit

$(BIT2MNF): bit2mnf.cpp ../code_table/cp932.h
	$(CXX) -o $@ $(CXXFLAGS) bit2mnf.cpp
	$(STRIP) $@

clean:
	rm -f $(TARGET) $(BIT2MNF)

install:
	bzip2 -c MONA-12.MNF > $(MONADIR)/bin/root/MONA-12.MF2
