// LZOutWindow.cpp

#include "StdAfx.h"

#include "LZOutWindow.h"

void CLZOutWindow::Create(UInt32 windowSize)
{
  _pos = 0;
  _streamPos = 0;
  const UInt32 kMinBlockSize = 1;
  if (windowSize < kMinBlockSize)
    windowSize = kMinBlockSize;
  if (_buffer != 0 && _windowSize == windowSize)
    return;
  Free();
  _windowSize = windowSize;
  #ifdef WIN32
  _buffer = (Byte *)::VirtualAlloc(0, windowSize, MEM_COMMIT, PAGE_READWRITE);
  if (_buffer == 0)
    throw 1; // CNewException();
  #else
  _buffer = new Byte[windowSize];
  #endif
}

void CLZOutWindow::Free()
{
  #ifdef WIN32
  if (_buffer != 0)
    VirtualFree(_buffer, 0, MEM_RELEASE);
  #else
  delete []_buffer;
  #endif
  _buffer = 0;
}

/*
void CLZOutWindow::SetWindowSize(UInt32 windowSize)
{
  _windowSize = windowSize;
}
*/

void CLZOutWindow::Init(ISequentialOutStream *stream, bool solid)
{
  // ReleaseStream();
  _stream = stream;
  // _stream->AddRef();

  if(!solid)
  {
    _streamPos = 0;
    _pos = 0;
  }
}

/*
void CLZOutWindow::ReleaseStream()
{
  if(_stream != 0)
  {
    // Flush(); // Test it
    _stream->Release();
    _stream = 0;
  }
}
*/

void CLZOutWindow::FlushWithCheck()
{
  HRESULT result = Flush();
  if (result != S_OK)
    throw CLZOutWindowException(result);
}

HRESULT CLZOutWindow::Flush()
{
  UInt32 size = _pos - _streamPos;
  if(size == 0)
    return S_OK;
  UInt32 processedSize;
  HRESULT result = _stream->Write(_buffer + _streamPos, size, &processedSize);
  if (result != S_OK)
    return result;
  if (size != processedSize)
    return E_FAIL;
  if (_pos >= _windowSize)
    _pos = 0;
  _streamPos = _pos;
  return S_OK;
}
