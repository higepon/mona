// OutBuffer.h

#ifndef __OUTBUFFER_H
#define __OUTBUFFER_H

#include "../IStream.h"

class COutBufferException
{
public:
  HRESULT ErrorCode;
  COutBufferException(HRESULT errorCode): ErrorCode(errorCode) {}
};

class COutBuffer
{
  Byte *_buffer;
  UInt32 _pos;
  UInt32 _bufferSize;
  ISequentialOutStream *_stream;
  UInt64 _processedSize;

  void WriteBlock();
public:
  COutBuffer(UInt32 bufferSize = (1 << 20));
  ~COutBuffer();

  void Init(ISequentialOutStream *stream);
  HRESULT Flush();
  // void ReleaseStream(); {  _stream.Release(); }

  void *GetBuffer(UInt32 &sizeAvail)
  {
    sizeAvail = _bufferSize - _pos;
    return _buffer + _pos;
  }
  void MovePos(UInt32 num)
  {
    _pos += num;
    if(_pos >= _bufferSize)
      WriteBlock();
  }

  void WriteByte(Byte b)
  {
    _buffer[_pos++] = b;
    if(_pos >= _bufferSize)
      WriteBlock();
  }
  void WriteBytes(const void *data, UInt32 size)
  {
    for (UInt32 i = 0; i < size; i++)
      WriteByte(((const Byte *)data)[i]);
  }

  UInt64 GetProcessedSize() const { return _processedSize + _pos; }
};

#endif
