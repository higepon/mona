// This file is in the public domain.
// There are no restrictions on any sort of usage of this file.

#ifdef MONA
#include <monapi.h>
#include <monapi/messages.h>

extern dword __gui_server;
#endif

#include <gui/messages.h>
#include <gui/System/Pointer.h>
#include <gui/System/Drawing/Bitmap.h>

namespace System { namespace Drawing
{
	Bitmap::Bitmap(int width, int height)
	{
		this->width  = width;
		this->height = height;
		int len = width * height;
		this->buffer.Alloc(len);
		Color white = Color::get_White();
		for (int i = 0; i < len; i++) this->buffer[i] = white;
	}
	
	Bitmap::Bitmap(String fileName)
	{
#ifdef MONA
		int fnlen = fileName.get_Length();
		if (fnlen > 127) fnlen = 127;
		_A<char> fn(fnlen + 1);
		for (int i = 0; i < fnlen; i++)
		{
			wchar ch = fileName[i];
			fn[i] = ch < 0x80 ? (char)ch : '?';
		}
		fn[fnlen] = '\0';
		
		MessageInfo msg;
		if (MonAPI::Message::sendReceive(&msg, __gui_server, MSG_GUISERVER_DECODEIMAGE, 0, 0, 0, fn.get()) != 0)
		{
			::printf("%s:%d:ERROR: can not connect to GUI server!\n", __FILE__, __LINE__);
			return;
		}
		if (msg.arg2 == 0) return;
		
		byte* image = MonAPI::MemoryMap::map(msg.arg2);
		if (image == NULL)
		{
			::printf("%s:%d:ERROR: can not get image data!\n", __FILE__, __LINE__);
			return;
		}
		
		this->width  = GET_X_DWORD(msg.arg3);
		this->height = GET_Y_DWORD(msg.arg3);
		int len = this->width * this->height;
		this->buffer.Alloc(len);
		::memcpy(this->buffer.get(), image, len * 4);
		MonAPI::MemoryMap::unmap(msg.arg2);
		
		if (MonAPI::Message::send(__gui_server, MSG_DISPOSE_HANDLE, msg.arg2) != 0)
		{
			::printf("%s:%d:ERROR: can not connect to GUI server!\n", __FILE__, __LINE__);
			return;
		}
#endif
	}
	
	Bitmap::~Bitmap()
	{
	}
	
	void Bitmap::SetPixel(int x, int y, Color c)
	{
		if (x < 0 || this->width <= x || y < 0 || this->height <= y) return;
		int index = x + y * this->width;
		Color v1 = this->buffer[index];
		Color v2 = c;
		if (c.get_A() != 0xff && c.ToArgb() != 0)
		{
			int a = c.get_A();
			int r = (v1.get_R() * (255 - a) + c.get_R() * a) / 255;
			int g = (v1.get_G() * (255 - a) + c.get_G() * a) / 255;
			int b = (v1.get_B() * (255 - a) + c.get_B() * a) / 255;
			v2 = Color::FromArgb(0xff, r, g, b);
		}
		this->buffer[index] = v2;
	}
}}
