// InBuffer.cpp

#include "StdAfx.h"

#include "InBuffer.h"

CInBuffer::CInBuffer(UInt32 bufferSize):
  _bufferSize(bufferSize),
  _bufferBase(0)
{
  _bufferBase = new Byte[_bufferSize];
}

CInBuffer::~CInBuffer()
{
  delete []_bufferBase;
}

void CInBuffer::Init(ISequentialInStream *stream)
{
  _stream = stream;
  _processedSize = 0;
  _buffer = _bufferBase;
  _bufferLimit = _buffer;
  _wasFinished = false;
}

bool CInBuffer::ReadBlock()
{
  if (_wasFinished)
    return false;
  _processedSize += (_buffer - _bufferBase);
  UInt32 numProcessedBytes;
  HRESULT result = _stream->ReadPart(_bufferBase, _bufferSize, &numProcessedBytes);
  if (result != S_OK)
    throw CInBufferException(result);
  _buffer = _bufferBase;
  _bufferLimit = _buffer + numProcessedBytes;
  _wasFinished = (numProcessedBytes == 0);
  return (!_wasFinished);
}
