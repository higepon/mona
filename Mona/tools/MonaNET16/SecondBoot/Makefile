include ../MakeInclude

TARGET = SecondBoot.exe
SRCS   = SecondBoot.cs Str.cs A20.cs FDC.cs VESA.cs

all: ../$(TARGET)

../$(TARGET): $(SRCS) ../I8086.dll
	$(CSC) /r:..$(SEP)I8086.dll /out:..$(SEP)$(TARGET) $(SRCS)

clean:
	rm -f ../$(TARGET)
