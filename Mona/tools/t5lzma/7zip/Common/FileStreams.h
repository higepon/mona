// FileStreams.h

#ifndef __FILESTREAMS_H
#define __FILESTREAMS_H

#ifdef WIN32
#include "../../Windows/FileIO.h"
#else
#include "../../Common/C_FileIO.h"
#endif

#include "../IStream.h"
#include "../../Common/MyCom.h"

class CInFileStream: 
  public IInStream,
  public IStreamGetSize,
  public CMyUnknownImp
{
public:
  #ifdef WIN32
  NWindows::NFile::NIO::CInFile File;
  #else
  NC::NFile::NIO::CInFile File;
  #endif
  CInFileStream() {}
  bool Open(LPCTSTR fileName);
  #ifdef WIN32
  #ifndef _UNICODE
  bool Open(LPCWSTR fileName);
  #endif
  #endif

  MY_UNKNOWN_IMP2(IInStream, IStreamGetSize)

  STDMETHOD(Read)(void *data, UInt32 size, UInt32 *processedSize);
  STDMETHOD(ReadPart)(void *data, UInt32 size, UInt32 *processedSize);
  STDMETHOD(Seek)(Int64 offset, UInt32 seekOrigin, UInt64 *newPosition);

  STDMETHOD(GetSize)(UInt64 *size);
};

class CStdInFileStream: 
  public ISequentialInStream,
  public CMyUnknownImp
{
public:
  // HANDLE File;
  // CStdInFileStream() File(INVALID_HANDLE_VALUE): {}
  // void Open() { File = GetStdHandle(STD_INPUT_HANDLE); };
  MY_UNKNOWN_IMP

  STDMETHOD(Read)(void *data, UInt32 size, UInt32 *processedSize);
  STDMETHOD(ReadPart)(void *data, UInt32 size, UInt32 *processedSize);
};


class COutFileStream: 
  public IOutStream,
  public CMyUnknownImp
{
public:
  #ifdef WIN32
  NWindows::NFile::NIO::COutFile File;
  #else
  NC::NFile::NIO::COutFile File;
  #endif
  COutFileStream() {}
  bool Open(LPCTSTR fileName);
  #ifdef WIN32
  #ifndef _UNICODE
  bool Open(LPCWSTR fileName);
  #endif
  #endif
  
  MY_UNKNOWN_IMP1(IOutStream)

  STDMETHOD(Write)(const void *data, UInt32 size, UInt32 *processedSize);
  STDMETHOD(WritePart)(const void *data, UInt32 size, UInt32 *processedSize);
  STDMETHOD(Seek)(Int64 offset, UInt32 seekOrigin, UInt64 *newPosition);
  STDMETHOD(SetSize)(Int64 newSize);
};

class CStdOutFileStream: 
  public ISequentialOutStream,
  public CMyUnknownImp
{
public:
  MY_UNKNOWN_IMP

  STDMETHOD(Write)(const void *data, UInt32 size, UInt32 *processedSize);
  STDMETHOD(WritePart)(const void *data, UInt32 size, UInt32 *processedSize);
};

#endif
