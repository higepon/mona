PROG=t5lzma$(EXEEXT)

include ../../../../../env/Makefile.inc

CXX = g++
CC  = gcc
LIB = -lstdc++ -lm
RM=-rm
CFLAGS = -c -O3 -UWIN32
LDFLAGS = -Wl,-s

OBJS = \
  LzmaAlone.o \
  LzmaBench.o \
  LZMADecoder.o \
  LZMAEncoder.o \
  LZInWindow.o \
  LZOutWindow.o \
  RangeCoderBit.o \
  InBuffer.o \
  OutBuffer.o \
  FileStreams.o \
  AlignedBuffer.o \
  C_FileIO.o \
  CommandLineParser.o \
  CRC.o \
  String.o \
  StringConvert.o \
  StringToInt.o \
  Vector.o
# FileIO.o

all: $(PROG)

$(PROG): $(OBJS)
	$(CXX) -o $(PROG) $(LDFLAGS) $(OBJS) $(LIB)
	strip $(PROG)
	cp $(PROG) ../../../../../bin/$(PROG)

LzmaAlone.o: LzmaAlone.cpp
	$(CXX) $(CFLAGS) LzmaAlone.cpp

LzmaBench.o: LzmaBench.cpp
	$(CXX) $(CFLAGS) LzmaBench.cpp

LZMADecoder.o: ../LZMA/LZMADecoder.cpp
	$(CXX) $(CFLAGS) ../LZMA/LZMADecoder.cpp

LZMAEncoder.o: ../LZMA/LZMAEncoder.cpp
	$(CXX) $(CFLAGS) ../LZMA/LZMAEncoder.cpp

LZInWindow.o: ../LZ/LZInWindow.cpp
	$(CXX) $(CFLAGS) ../LZ/LZInWindow.cpp

LZOutWindow.o: ../LZ/LZOutWindow.cpp
	$(CXX) $(CFLAGS) ../LZ/LZOutWindow.cpp

RangeCoderBit.o: ../RangeCoder/RangeCoderBit.cpp
	$(CXX) $(CFLAGS) ../RangeCoder/RangeCoderBit.cpp

InBuffer.o: ../../Common/InBuffer.cpp
	$(CXX) $(CFLAGS) ../../Common/InBuffer.cpp

OutBuffer.o: ../../Common/OutBuffer.cpp
	$(CXX) $(CFLAGS) ../../Common/OutBuffer.cpp

FileStreams.o: ../../Common/FileStreams.cpp
	$(CXX) $(CFLAGS) ../../Common/FileStreams.cpp

AlignedBuffer.o: ../../../Common/AlignedBuffer.cpp
	$(CXX) $(CFLAGS) ../../../Common/AlignedBuffer.cpp

C_FileIO.o: ../../../Common/C_FileIO.cpp
	$(CXX) $(CFLAGS) ../../../Common/C_FileIO.cpp

CommandLineParser.o: ../../../Common/CommandLineParser.cpp
	$(CXX) $(CFLAGS) ../../../Common/CommandLineParser.cpp

CRC.o: ../../../Common/CRC.cpp
	$(CXX) $(CFLAGS) ../../../Common/CRC.cpp

#MyWindows.o: ../../../Common/MyWindows.cpp
#	$(CXX) $(CFLAGS) ../../../Common/MyWindows.cpp

String.o: ../../../Common/String.cpp
	$(CXX) $(CFLAGS) ../../../Common/String.cpp

StringConvert.o: ../../../Common/StringConvert.cpp
	$(CXX) $(CFLAGS) ../../../Common/StringConvert.cpp

StringToInt.o: ../../../Common/StringToInt.cpp
	$(CXX) $(CFLAGS) ../../../Common/StringToInt.cpp

Vector.o: ../../../Common/Vector.cpp
	$(CXX) $(CFLAGS) ../../../Common/Vector.cpp

FileIO.o: ../../../Windows/FileIO.cpp
	$(CXX) $(CFLAGS) ../../../Windows/FileIO.cpp

clean:
	-$(RM) $(PROG) $(OBJS)

