// This file is in the public domain.
// There are no restrictions on any sort of usage of this file.

#ifdef MONA
#include <monapi.h>

enum
{
	MSG_GUISERVER_GETFONT = 0x4000,
	MSG_GUISERVER_RETURNFONT,
	MSG_GUISERVER_DECODEIMAGE,
	MSG_GUISERVER_RETURNIMAGE,
	MSG_GUISERVER_DISPOSEIMAGE
};

extern int __SendMessage(dword to, dword header, dword arg1, dword arg2, dword arg3, const char* str = NULL);
extern MessageInfo __WaitMessage(dword header);

extern dword __gui_server;
#endif

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
		
		if (::__SendMessage(__gui_server, MSG_GUISERVER_DECODEIMAGE, 0, 0, 0, fn.get()) != 0)
		{
			::printf("ERROR: Can't connect to GUI server!\n");
			::exit(1);
		}
		MessageInfo msg = __WaitMessage(MSG_GUISERVER_RETURNIMAGE);
		if (msg.arg1 == 0) return;
		
		byte* image = MonAPI::MemoryMap::map(msg.arg1);
		if (image == NULL)
		{
			::printf("ERROR: Can not get image data!\n");
			::exit(1);
		}
		
		this->width  = msg.arg2;
		this->height = msg.arg3;
		int len = this->width * this->height;
		this->buffer.Alloc(len);
		::memcpy(this->buffer.get(), image, len * 4);
		MonAPI::MemoryMap::unmap(msg.arg1);
		
		if (::__SendMessage(__gui_server, MSG_GUISERVER_DISPOSEIMAGE, msg.arg1, 0, 0) != 0)
		{
			::printf("ERROR: Can't connect to GUI server!\n");
			::exit(1);
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
