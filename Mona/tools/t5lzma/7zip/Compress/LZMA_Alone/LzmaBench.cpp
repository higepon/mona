// LzmaBench.cpp

#include "StdAfx.h"

#include <stdio.h>

#ifndef WIN32
#include <time.h>
#endif

#include "../../../Common/CRC.h"
#include "../LZMA/LZMADecoder.h"
#include "../LZMA/LZMAEncoder.h"

static const UInt32 kAdditionalSize = (6 << 20);
static const UInt32 kCompressedAdditionalSize = (1 << 10);
static const UInt32 kMaxLzmaPropSize = 10;

class CRandomGenerator
{
  UInt32 A1;
  UInt32 A2;
public:
  CRandomGenerator() { Init(); }
  void Init() { A1 = 362436069; A2 = 521288629;}
  UInt32 GetRnd() 
  {
    return 
      ((A1 = 36969 * (A1 & 0xffff) + (A1 >> 16)) << 16) ^
      ((A2 = 18000 * (A2 & 0xffff) + (A2 >> 16)) );
  }
};

class CBitRandomGenerator
{
  CRandomGenerator RG;
  UInt32 Value;
  int NumBits;
public:
  void Init()
  {
    Value = 0;
    NumBits = 0;
  }
  UInt32 GetRnd(int numBits) 
  {
    if (NumBits > numBits)
    {
      UInt32 result = Value & ((1 << numBits) - 1);
      Value >>= numBits;
      NumBits -= numBits;
      return result;
    }
    numBits -= NumBits;
    UInt32 result = (Value << numBits);
    Value = RG.GetRnd();
    result |= Value & ((1 << numBits) - 1);
    Value >>= numBits;
    NumBits = 32 - numBits;
    return result;
  }
};

class CBenchRandomGenerator
{
  CBitRandomGenerator RG;
  UInt32 Pos;
public:
  UInt32 BufferSize;
  Byte *Buffer;
  CBenchRandomGenerator(): Buffer(0) {} 
  ~CBenchRandomGenerator() { delete []Buffer; }
  void Init() { RG.Init(); }
  void Set(UInt32 bufferSize) 
  {
    delete []Buffer;
    Buffer = 0;
    Buffer = new Byte[bufferSize];
    Pos = 0;
    BufferSize = bufferSize;
  }
  UInt32 GetRndBit() { return RG.GetRnd(1); }
  /*
  UInt32 GetLogRand(int maxLen)
  {
    UInt32 len = GetRnd() % (maxLen + 1);
    return GetRnd() & ((1 << len) - 1);
  }
  */
  UInt32 GetLogRandBits(int numBits)
  {
    UInt32 len = RG.GetRnd(numBits);
    return RG.GetRnd(len);
  }
  UInt32 GetOffset()
  {
    if (GetRndBit() == 0)
      return GetLogRandBits(4);
    return (GetLogRandBits(4) << 10) | RG.GetRnd(10);
  }
  UInt32 GetLen()
  {
    if (GetRndBit() == 0)
      return RG.GetRnd(2);
    if (GetRndBit() == 0)
      return 4 + RG.GetRnd(3);
    return 12 + RG.GetRnd(4);
  }
  void Generate()
  {
    while(Pos < BufferSize)
    {
      if (GetRndBit() == 0 || Pos < 1)
        Buffer[Pos++] = Byte(RG.GetRnd(8));
      else
      {
        UInt32 offset = GetOffset();
        while (offset >= Pos)
          offset >>= 1;
        offset += 1;
        UInt32 len = 2 + GetLen();
        for (UInt32 i = 0; i < len && Pos < BufferSize; i++, Pos++)
          Buffer[Pos] = Buffer[Pos - offset];
      }
    }
  }
};

class CBenchmarkInStream: 
  public ISequentialInStream,
  public CMyUnknownImp
{
  const Byte *Data;
  UInt32 Pos;
  UInt32 Size;
public:
  MY_UNKNOWN_IMP
  void Init(const Byte *data, UInt32 size)
  {
    Data = data;
    Size = size;
    Pos = 0;
  }
  STDMETHOD(Read)(void *data, UInt32 size, UInt32 *processedSize);
  STDMETHOD(ReadPart)(void *data, UInt32 size, UInt32 *processedSize);
};

STDMETHODIMP CBenchmarkInStream::Read(void *data, UInt32 size, UInt32 *processedSize)
{
  UInt32 remain = Size - Pos;
  if (size > remain)
    size = remain;
  for (UInt32 i = 0; i < size; i++)
  {
    ((Byte *)data)[i] = Data[Pos + i];
  }
  Pos += size;
  if(processedSize != NULL)
    *processedSize = size;
  return S_OK;
}
  
STDMETHODIMP CBenchmarkInStream::ReadPart(void *data, UInt32 size, UInt32 *processedSize)
{
  return Read(data, size, processedSize);
}

class CBenchmarkOutStream: 
  public ISequentialOutStream,
  public CMyUnknownImp
{
  UInt32 BufferSize;
public:
  UInt32 Pos;
  Byte *Buffer;
  CBenchmarkOutStream(): Buffer(0) {} 
  ~CBenchmarkOutStream() { delete []Buffer; }
  void Init(UInt32 bufferSize) 
  {
    delete []Buffer;
    Buffer = 0;
    Buffer = new Byte[bufferSize];
    Pos = 0;
    BufferSize = bufferSize;
  }
  MY_UNKNOWN_IMP
  STDMETHOD(Write)(const void *data, UInt32 size, UInt32 *processedSize);
  STDMETHOD(WritePart)(const void *data, UInt32 size, UInt32 *processedSize);
};

STDMETHODIMP CBenchmarkOutStream::Write(const void *data, UInt32 size, UInt32 *processedSize)
{
  UInt32 i;
  for (i = 0; i < size && Pos < BufferSize; i++)
    Buffer[Pos++] = ((const Byte *)data)[i];
  if(processedSize != NULL)
    *processedSize = i;
  if (i != size)
  {
    fprintf(stderr, "\nERROR: Buffer is full\n");
    return E_FAIL;
  }
  return S_OK;
}
  
STDMETHODIMP CBenchmarkOutStream::WritePart(const void *data, UInt32 size, UInt32 *processedSize)
{
  return Write(data, size, processedSize);
}

class CCrcOutStream: 
  public ISequentialOutStream,
  public CMyUnknownImp
{
public:
  CCRC CRC;
  MY_UNKNOWN_IMP
  void Init() { CRC.Init(); }
  STDMETHOD(Write)(const void *data, UInt32 size, UInt32 *processedSize);
  STDMETHOD(WritePart)(const void *data, UInt32 size, UInt32 *processedSize);
};

STDMETHODIMP CCrcOutStream::Write(const void *data, UInt32 size, UInt32 *processedSize)
{
  CRC.Update(data, size);
  if(processedSize != NULL)
    *processedSize = size;
  return S_OK;
}
  
STDMETHODIMP CCrcOutStream::WritePart(const void *data, UInt32 size, UInt32 *processedSize)
{
  return Write(data, size, processedSize);
}

static UInt64 GetTimeCount()
{
  #ifdef WIN32
  LARGE_INTEGER value;
  if (::QueryPerformanceCounter(&value))
    return value.QuadPart;
  return GetTickCount();
  #else
  return clock();
  #endif 
}

static UInt64 GetFreq()
{
  #ifdef WIN32
  LARGE_INTEGER value;
  if (::QueryPerformanceFrequency(&value))
    return value.QuadPart;
  return 1000;
  #else
  return CLOCKS_PER_SEC;
  #endif 
}

struct CProgressInfo:
  public ICompressProgressInfo,
  public CMyUnknownImp
{
  UInt64 ApprovedStart;
  UInt64 InSize;
  UInt64 Time;
  void Init()
  {
    InSize = 0;
    Time = 0;
  }
  MY_UNKNOWN_IMP
  STDMETHOD(SetRatioInfo)(const UInt64 *inSize, const UInt64 *outSize);
};

STDMETHODIMP CProgressInfo::SetRatioInfo(const UInt64 *inSize, const UInt64 *outSize)
{
  if (*inSize >= ApprovedStart && InSize == 0)
  {
    Time = ::GetTimeCount();
    InSize = *inSize;
  }
  return S_OK;
}

static const int kSubBits = 8;

static UInt32 GetLogSize(UInt32 size)
{
  for (int i = kSubBits; i < 32; i++)
    for (UInt32 j = 0; j < (1 << kSubBits); j++)
      if (size <= (((UInt32)1) << i) + (j << (i - kSubBits)))
        return (i << kSubBits) + j;
  return (32 << kSubBits);
}

static UInt64 GetCompressRating(UInt32 dictionarySize, 
    UInt64 elapsedTime, UInt64 size)
{
  if (elapsedTime == 0)
    elapsedTime = 1;
  UInt64 t = GetLogSize(dictionarySize) - (19 << kSubBits);
  UInt64 numCommandsForOne = 2000 + ((t * t * 68) >> (2 * kSubBits));
  UInt64 numCommands = (UInt64)(size) * numCommandsForOne;
  return numCommands * GetFreq() / elapsedTime;
}

static UInt64 GetDecompressRating(UInt64 elapsedTime, 
    UInt64 outSize, UInt64 inSize)
{
  if (elapsedTime == 0)
    elapsedTime = 1;
  UInt64 numCommands = inSize * 250 + outSize * 21;
  return numCommands * GetFreq() / elapsedTime;
}

static UInt64 GetTotalRating(
    UInt32 dictionarySize, 
    UInt64 elapsedTimeEn, UInt64 sizeEn,
    UInt64 elapsedTimeDe, 
    UInt64 inSizeDe, UInt64 outSizeDe)
{
  return (GetCompressRating(dictionarySize, elapsedTimeEn, sizeEn) + 
    GetDecompressRating(elapsedTimeDe, inSizeDe, outSizeDe)) / 2;
}

static void PrintRating(UInt64 rating)
{
  fprintf(stderr, "%5d MIPS", (unsigned int)(rating / 1000000));
}

static void PrintResults(
    UInt32 dictionarySize,
    UInt64 elapsedTime, 
    UInt64 size, 
    bool decompressMode, UInt64 secondSize)
{
  if (elapsedTime == 0)
    elapsedTime = 1;
  UInt64 speed = size * GetFreq() / elapsedTime;
  fprintf(stderr, "%6d KB/s  ", (unsigned int)(speed / 1024));
  UInt64 rating;
  if (decompressMode)
    rating = GetDecompressRating(elapsedTime, size, secondSize);
  else
    rating = GetCompressRating(dictionarySize, elapsedTime, size);
  PrintRating(rating);
}

static void ThrowError(HRESULT result)
{
  if (result == E_ABORT)
    throw "User break";
  if (result == E_OUTOFMEMORY)
    throw "Can not allocate memory";
}

int LzmaBenchmark(UInt32 numIterations, UInt32 dictionarySize)
{
  if (numIterations == 0)
    return 0;
  if (dictionarySize < (1 << 19))
    throw "dictionary size for benchmark must be >= 19 (512 KB)";
  fprintf(stderr, "\n       Compressing                Decompressing\n\n");
  NCompress::NLZMA::CEncoder *encoderSpec = new NCompress::NLZMA::CEncoder;
  CMyComPtr<ICompressCoder> encoder = encoderSpec;

  NCompress::NLZMA::CDecoder *decoderSpec = new NCompress::NLZMA::CDecoder;
  CMyComPtr<ICompressCoder> decoder = decoderSpec;

  CBenchmarkOutStream *propStreamSpec = new CBenchmarkOutStream;
  CMyComPtr<ISequentialOutStream> propStream = propStreamSpec;
  propStreamSpec->Init(kMaxLzmaPropSize);
  
  CBenchmarkInStream *propDecoderStreamSpec = new CBenchmarkInStream;
  CMyComPtr<ISequentialInStream> propDecoderStream = propDecoderStreamSpec;

  PROPID propIDs[] = { NCoderPropID::kDictionarySize };
  const int kNumProps = sizeof(propIDs) / sizeof(propIDs[0]);
  PROPVARIANT properties[kNumProps];
  properties[0].vt = VT_UI4;
  properties[0].ulVal = UInt32(dictionarySize);

  const UInt32 kBufferSize = dictionarySize + kAdditionalSize;
  const UInt32 kCompressedBufferSize = (kBufferSize / 2) + kCompressedAdditionalSize;

  if (encoderSpec->SetCoderProperties(propIDs, properties, kNumProps) != S_OK)
    throw "Incorrect command";
  encoderSpec->WriteCoderProperties(propStream);

  CBenchRandomGenerator rg;
  rg.Init();
  rg.Set(kBufferSize);
  rg.Generate();
  CCRC crc;
  crc.Update(rg.Buffer, rg.BufferSize);

  CProgressInfo *progressInfoSpec = new CProgressInfo;
  CMyComPtr<ICompressProgressInfo> progressInfo = progressInfoSpec;

  progressInfoSpec->ApprovedStart = dictionarySize;

  UInt64 totalBenchSize = 0;
  UInt64 totalEncodeTime = 0;
  UInt64 totalDecodeTime = 0;
  UInt64 totalCompressedSize = 0;

  for (UInt32 i = 0; i < numIterations; i++)
  {
    progressInfoSpec->Init();
    CBenchmarkInStream *inStreamSpec = new CBenchmarkInStream;
    inStreamSpec->Init(rg.Buffer, rg.BufferSize);
    CMyComPtr<ISequentialInStream> inStream = inStreamSpec;
    CBenchmarkOutStream *outStreamSpec = new CBenchmarkOutStream;
    outStreamSpec->Init(kCompressedBufferSize);
    CMyComPtr<ISequentialOutStream> outStream = outStreamSpec;
    HRESULT result = encoder->Code(inStream, outStream, 0, 0, progressInfo);
    UInt64 encodeTime = ::GetTimeCount() - progressInfoSpec->Time;
    UInt32 compressedSize = outStreamSpec->Pos;
    if(result != S_OK)
    {
      ThrowError(result);
      throw "Encode Error";
    }
    if (progressInfoSpec->InSize == 0)
      throw "Internal ERROR 1282";
  
    ///////////////////////
    // Decompressing
  
    CCrcOutStream *crcOutStreamSpec = new CCrcOutStream;
    CMyComPtr<ISequentialOutStream> crcOutStream = crcOutStreamSpec;
    
    UInt64 decodeTime;
    for (int i = 0; i < 2; i++)
    {
      inStreamSpec->Init(outStreamSpec->Buffer, compressedSize);
      crcOutStreamSpec->Init();
      
      propDecoderStreamSpec->Init(propStreamSpec->Buffer, propStreamSpec->Pos);
      if (decoderSpec->SetDecoderProperties(propDecoderStream) != S_OK)
        throw "Set Decoder Properties Error";
      UInt64 outSize = kBufferSize;
      UInt64 startTime = ::GetTimeCount();
      result = decoder->Code(inStream, crcOutStream, 0, &outSize, 0);
      decodeTime = ::GetTimeCount() - startTime;
      if(result != S_OK)
      {
        ThrowError(result);
        throw "Decode Error";
      }
      if (crcOutStreamSpec->CRC.GetDigest() != crc.GetDigest())
        throw "CRC Error";
    }
    UInt64 benchSize = kBufferSize - progressInfoSpec->InSize;
    PrintResults(dictionarySize, encodeTime, benchSize, false, 0);
    fprintf(stderr, "     ");
    PrintResults(dictionarySize, decodeTime, kBufferSize, true, compressedSize);
    fprintf(stderr, "\n");

    totalBenchSize += benchSize;
    totalEncodeTime += encodeTime;
    totalDecodeTime += decodeTime;
    totalCompressedSize += compressedSize;
  }
  fprintf(stderr, "---------------------------------------------------\n");
  PrintResults(dictionarySize, totalEncodeTime, totalBenchSize, false, 0);
  fprintf(stderr, "     ");
  PrintResults(dictionarySize, totalDecodeTime, 
      kBufferSize * numIterations, true, totalCompressedSize);
  fprintf(stderr, "    Average\n");
  return 0;
}
