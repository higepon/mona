// LZOutWindow.h

#ifndef __LZ_OUT_WINDOW_H
#define __LZ_OUT_WINDOW_H

#include "../../IStream.h"

class CLZOutWindowException
{
public:
  HRESULT ErrorCode;
  CLZOutWindowException(HRESULT errorCode): ErrorCode(errorCode) {}
};

class CLZOutWindow
{
  Byte  *_buffer;
  UInt32 _pos;
  UInt32 _windowSize;
  UInt32 _streamPos;
  ISequentialOutStream *_stream;
  void FlushWithCheck();
  void Free();
public:
  CLZOutWindow(): _buffer(0), _stream(0) {}
  ~CLZOutWindow() { Free();  /* ReleaseStream(); */ }
	void Create(UInt32 windowSize);
  bool IsCreated() const { return _buffer != 0; }

  void Init(ISequentialOutStream *stream, bool solid = false);
  HRESULT Flush();
  // void ReleaseStream();
  
  void CopyBlock(UInt32 distance, UInt32 len)
  {
		UInt32 pos = _pos - distance - 1;
  	if (pos >= _windowSize)
  		pos += _windowSize;
		for(; len > 0; len--)
		{
			if (pos >= _windowSize)
				pos = 0;
			_buffer[_pos++] = _buffer[pos++];
			if (_pos >= _windowSize)
				FlushWithCheck();  
			// PutOneByte(GetOneByte(distance));
		}
  }

  void PutByte(Byte b)
  {
		_buffer[_pos++] = b;
		if (_pos >= _windowSize)
			FlushWithCheck();  
  }

  Byte GetByte(UInt32 distance) const
  {
		UInt32 pos = _pos - distance - 1;
		if (pos >= _windowSize)
			pos += _windowSize;
		return _buffer[pos]; 
  }
};

#endif
