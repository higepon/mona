// This file is in the public domain.
// There are no restrictions on any sort of usage of this file.

#include <gui/System/Drawing/Bitmap.h>

namespace System { namespace Drawing
{
	Bitmap::Bitmap(int width, int height)
	{
		this->width  = width;
		this->height = height;
		int len = width * height;
		this->buffer.Set(new Color[len], len);
		Color white = Color::get_White();
		for (int i = 0; i < len; i++) this->buffer[i] = white;
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
