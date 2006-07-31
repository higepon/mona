SRCTOP=../..
include $(SRCTOP)/Makefile.inc

TARGET = PEAnalyzerLib.dll
SRCS   = \
	AssemblyInfo.cs \
	CodedIndices.cs \
	HeaderBase.cs \
	IndexManager.cs \
	MetadataTables.cs \
	MethodData.cs \
	PEData.cs \
	PEHeaders.cs \
	RVAManager.cs \
	StreamHeader.cs \
	TableBase.cs \
	Types.cs \
	Util.cs

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CSC) /out:$(TARGET) /target:library $(SRCS)

clean:
	rm -f $(TARGET)

distclean: clean

install: $(TARGET)
	mkdir -p $(DOTNETDIR)
	cp $(TARGET) Makefile.inc $(DOTNETDIR)/

uninstall:
	rm -f $(MONADIR)/share/dotnet/$(TARGET)
	rm -f $(MONADIR)/share/dotnet/Makefile.inc
