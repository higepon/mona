// LZInWindow.cpp

#include "StdAfx.h"

#include "LZInWindow.h"
#include "../../../Common/MyCom.h"

void CLZInWindow::Free()
{
  #ifdef WIN32
  if (_bufferBase != 0)
    VirtualFree(_bufferBase, 0, MEM_RELEASE);
  #else
  delete []_bufferBase;
  #endif
  _bufferBase = 0;
}

void CLZInWindow::Create(UInt32 keepSizeBefore, UInt32 keepSizeAfter, UInt32 keepSizeReserv)
{
  _keepSizeBefore = keepSizeBefore;
  _keepSizeAfter = keepSizeAfter;
  _keepSizeReserv = keepSizeReserv;
  UInt32 blockSize = keepSizeBefore + keepSizeAfter + keepSizeReserv;
  if (_bufferBase == 0 || _blockSize != blockSize)
  {
    Free();
    _blockSize = blockSize;
    #ifdef WIN32
    _bufferBase = (Byte *)::VirtualAlloc(0, _blockSize, MEM_COMMIT, PAGE_READWRITE);
    if (_bufferBase == 0)
      throw 1; // CNewException();
    #else
    _bufferBase = new Byte[_blockSize];
    #endif
  }
  _pointerToLastSafePosition = _bufferBase + _blockSize - keepSizeAfter;
}


HRESULT CLZInWindow::Init(ISequentialInStream *stream)
{
  _stream = stream;
  _buffer = _bufferBase;
  _pos = 0;
  _streamPos = 0;
  _streamEndWasReached = false;
  return ReadBlock();
}

/*
void CLZInWindow::ReleaseStream()
{
  _stream.Release();
}
*/

///////////////////////////////////////////
// ReadBlock

// In State:
//   (_buffer + _streamPos) <= (_bufferBase + _blockSize)
// Out State:
//   _posLimit <= _blockSize - _keepSizeAfter;
//   if(_streamEndWasReached == false):
//     _streamPos >= _pos + _keepSizeAfter
//     _posLimit = _streamPos - _keepSizeAfter;
//   else
//          
  
HRESULT CLZInWindow::ReadBlock()
{
  if(_streamEndWasReached)
    return S_OK;
  while(true)
  {
    UInt32 size = UInt32(_bufferBase - _buffer) + _blockSize - _streamPos;
    if(size == 0)
      return S_OK;
    UInt32 numReadBytes;
    RINOK(_stream->ReadPart(_buffer + _streamPos, size, &numReadBytes));
    if(numReadBytes == 0)
    {
      _posLimit = _streamPos;
      const Byte *pointerToPostion = _buffer + _posLimit;
      if(pointerToPostion > _pointerToLastSafePosition)
        _posLimit = (UInt32)(_pointerToLastSafePosition - _buffer);
      _streamEndWasReached = true;
      return S_OK;
    }
    _streamPos += numReadBytes;
    if(_streamPos >= _pos + _keepSizeAfter)
    {
      _posLimit = _streamPos - _keepSizeAfter;
      return S_OK;
    }
  }
}

void CLZInWindow::MoveBlock()
{
  BeforeMoveBlock();
  UInt32 offset = UInt32(_buffer - _bufferBase) + _pos - _keepSizeBefore;
  UInt32 numBytes = UInt32(_buffer - _bufferBase) + _streamPos -  offset;
  memmove(_bufferBase, _bufferBase + offset, numBytes);
  _buffer -= offset;
  AfterMoveBlock();
}
