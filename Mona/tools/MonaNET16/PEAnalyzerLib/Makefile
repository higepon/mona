include ../MakeInclude

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

all: ../$(TARGET)

../$(TARGET): $(SRCS)
	$(CSC) /out:..$(SEP)$(TARGET) /t:library $(SRCS)

clean:
	rm -f ../$(TARGET)
