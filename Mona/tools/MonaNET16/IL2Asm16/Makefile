include ../MakeInclude

TARGET = IL2Asm16.exe
SRCS   = IL2Asm16.cs X86Code.cs X86Codes.cs Util.cs

all: ../$(TARGET)

../$(TARGET): $(SRCS)
	$(CSC) /r:..$(SEP)PEAnalyzerLib.dll /out:..$(SEP)$(TARGET) /debug $(SRCS)

clean:
	rm -f ../$(TARGET) ../IL2Asm16.pdb
