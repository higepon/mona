// FileStreams.cpp

#include "StdAfx.h"

#include "FileStreams.h"

static inline HRESULT ConvertBoolToHRESULT(bool result)
{
  // return result ? S_OK: E_FAIL;
  #ifdef WIN32
  return result ? S_OK: (::GetLastError());
  #else
  return result ? S_OK: E_FAIL;
  #endif
}

bool CInFileStream::Open(LPCTSTR fileName)
{
  return File.Open(fileName);
}

#ifdef WIN32
#ifndef _UNICODE
bool CInFileStream::Open(LPCWSTR fileName)
{
  return File.Open(fileName);
}
#endif
#endif

STDMETHODIMP CInFileStream::Read(void *data, UInt32 size, UInt32 *processedSize)
{
  UInt32 realProcessedSize;
  bool result = File.Read(data, size, realProcessedSize);
  if(processedSize != NULL)
    *processedSize = realProcessedSize;
  return ConvertBoolToHRESULT(result);
}
  
STDMETHODIMP CInFileStream::ReadPart(void *data, UInt32 size, UInt32 *processedSize)
{
  return Read(data, size, processedSize);
}

STDMETHODIMP CStdInFileStream::Read(void *data, UInt32 size, UInt32 *processedSize)
{
  UInt32 realProcessedSize;
  #ifdef WIN32
  BOOL res = ::ReadFile(GetStdHandle(STD_INPUT_HANDLE), 
      data, size, (DWORD *)&realProcessedSize, NULL);
  if(processedSize != NULL)
    *processedSize = realProcessedSize;
  if (res == FALSE && GetLastError() == ERROR_BROKEN_PIPE)
    return S_OK;
  return ConvertBoolToHRESULT(res != FALSE);
  #else
  realProcessedSize = fread(data, 1, size, stdin);
  if (processedSize != 0)
    *processedSize = realProcessedSize;
  if (ferror(stdin) != 0)
    return E_FAIL;
  return S_OK;
  #endif
}
  
STDMETHODIMP CStdInFileStream::ReadPart(void *data, UInt32 size, UInt32 *processedSize)
{
  return Read(data, size, processedSize);
}


STDMETHODIMP CInFileStream::Seek(Int64 offset, UInt32 seekOrigin, 
    UInt64 *newPosition)
{
  if(seekOrigin >= 3)
    return STG_E_INVALIDFUNCTION;
  #ifdef WIN32
  UInt64 realNewPosition;
  bool result = File.Seek(offset, seekOrigin, realNewPosition);
  if(newPosition != NULL)
    *newPosition = realNewPosition;
  return ConvertBoolToHRESULT(result);
  #else
  if (!File.Seek(offset, seekOrigin))
    return E_FAIL;
  if(newPosition != NULL)
  {
    if (!File.GetPosition(*newPosition))
      return E_FAIL;
  }
  return S_OK;
  #endif
}

STDMETHODIMP CInFileStream::GetSize(UInt64 *size)
{
  return ConvertBoolToHRESULT(File.GetLength(*size));
}


//////////////////////////
// COutFileStream

bool COutFileStream::Open(LPCTSTR fileName)
{
  #ifdef WIN32
  File.SetOpenCreationDispositionCreateAlways();
  #endif
  return File.Open(fileName);
}

#ifdef WIN32
#ifndef _UNICODE
bool COutFileStream::Open(LPCWSTR fileName)
{
  File.SetOpenCreationDispositionCreateAlways();
  return File.Open(fileName);
}
#endif
#endif

STDMETHODIMP COutFileStream::Write(const void *data, UInt32 size, UInt32 *processedSize)
{
  UInt32 realProcessedSize;
  bool result = File.Write(data, size, realProcessedSize);
  if(processedSize != NULL)
    *processedSize = realProcessedSize;
  return ConvertBoolToHRESULT(result);
}
  
STDMETHODIMP COutFileStream::WritePart(const void *data, UInt32 size, UInt32 *processedSize)
{
  return Write(data, size, processedSize);
}


STDMETHODIMP COutFileStream::Seek(Int64 offset, UInt32 seekOrigin, 
    UInt64 *newPosition)
{
  if(seekOrigin >= 3)
    return STG_E_INVALIDFUNCTION;
  #ifdef WIN32
  UInt64 realNewPosition;
  bool result = File.Seek(offset, seekOrigin, realNewPosition);
  if(newPosition != NULL)
    *newPosition = realNewPosition;
  return ConvertBoolToHRESULT(result);
  #else
  if (!File.Seek(offset, seekOrigin))
    return E_FAIL;
  if(newPosition != NULL)
  {
    if (!File.GetPosition(*newPosition))
      return E_FAIL;
  }
  return S_OK;
  #endif
}

STDMETHODIMP COutFileStream::SetSize(Int64 newSize)
{
  #ifdef WIN32
  UInt64 currentPos;
  if(!File.Seek(0, FILE_CURRENT, currentPos))
    return E_FAIL;
  bool result = File.SetLength(newSize);
  UInt64 currentPos2;
  result = result && File.Seek(currentPos, currentPos2);
  return result ? S_OK : E_FAIL;
  #else
  return E_FAIL;
  #endif
}

STDMETHODIMP CStdOutFileStream::Write(const void *data, UInt32 size, UInt32 *processedSize)
{
  if(processedSize != NULL)
    *processedSize = 0;
  UInt32 realProcessedSize;
  #ifdef WIN32
  BOOL res = TRUE;
  while (size > 0)
  {
    // Seems that Windows doesn't like big amounts writing to stdout.
    // So we limit portions by 32KB.
    UInt32 sizeTemp = (1 << 15); 
    if (sizeTemp > size)
      sizeTemp = size;
    res = ::WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), 
        data, sizeTemp, (DWORD *)&realProcessedSize, NULL);
    if (realProcessedSize == 0)
      break;
    size -= realProcessedSize;
    data = (const void *)((const Byte *)data + realProcessedSize);
    if(processedSize != NULL)
      *processedSize += realProcessedSize;
  }
  return ConvertBoolToHRESULT(res != FALSE);
  #else
  realProcessedSize = fwrite(data, 1, size, stdout);
  if (processedSize != 0)
    *processedSize = realProcessedSize;
  if (ferror(stdout) != 0)
    return E_FAIL;
  return S_OK;
  #endif
}
  
STDMETHODIMP CStdOutFileStream::WritePart(const void *data, UInt32 size, UInt32 *processedSize)
{
  return Write(data, size, processedSize);
}
