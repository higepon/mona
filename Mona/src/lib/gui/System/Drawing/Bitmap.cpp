// This file is in the public domain.
// There are no restrictions on any sort of usage of this file.

#ifdef MONA
#include <monapi.h>
#endif

#include <gui/System/Pointer.h>
#include <gui/System/Drawing/Bitmap.h>

#define BYTE2INT(array, index) (array[index] + (array[index + 1] << 8) + (array[index + 2] << 16) + (array[index + 3] << 24))

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
		_A<char> fn(fnlen + 1);
		for (int i = 0; i < fnlen; i++)
		{
			wchar ch = fileName[i];
			fn[i] = ch < 0x80 ? (char)ch : '?';
		}
		fn[fnlen] = '\0';
		_P<MonAPI::FileInputStream> fis(new MonAPI::FileInputStream(fn.get()), true);
		if (fis->open() != 0) return;
		
		int sz = fis->getFileSize();
		_A<byte> buf(sz);
		fis->read(buf.get(), sz);
		fis->close();
		if (sz < 6) return;
		
		if (buf[0] != 'B' || buf[1] != 'M') return;
		if (sz != BYTE2INT(buf, 2)) return;
		if (BYTE2INT(buf, 6) != 0) return;
		if (BYTE2INT(buf, 14) != 40) return;
		if (buf[28] + (buf[29] << 8) != 24) return;
		if (BYTE2INT(buf, 30) != 0) return;
		
		int bfOffBits = BYTE2INT(buf, 10);
		this->width  = BYTE2INT(buf, 18);
		this->height = BYTE2INT(buf, 22);
		this->buffer.Alloc(this->width * this->height);
		
		int p1 = 0;
		int lineSize = (this->width * 3 + 3) >> 2 << 2;
		for (int y = 0; y < this->height; y++)
		{
			int p2 = bfOffBits + (this->height - y - 1) * lineSize;
			for (int x = 0; x < this->width; x++, p1++, p2 += 3)
			{
				this->buffer[p1] = Color::FromArgb(buf[p2 + 2], buf[p2 + 1], buf[p2]);
			}
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
