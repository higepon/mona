// This file is in the public domain.
// There are no restrictions on any sort of usage of this file.

#ifndef __SYSTEM_DRAWING_BITMAP_H__
#define __SYSTEM_DRAWING_BITMAP_H__

#include <gui/System/Object.h>
#include <gui/System/Array.h>
#include <gui/System/Drawing/Color.h>

namespace System { namespace Drawing
{
	class Bitmap : public System::Object
	{
	protected:
		int width, height;
		_A<Color> buffer;
	
	public:
		virtual String get_TypeName() { return "System.Drawing.Bitmap"; }
		Bitmap(int width, int height);
		Bitmap(String fileName);
		virtual ~Bitmap();
		
		inline int get_Width() { return this->width; }
		inline int get_Height() { return this->height; }
		inline Color& operator [](int index) { return this->buffer[index]; }
		inline Color* get() { return this->buffer.get(); }
	
		void SetPixel(int x, int y, Color c);
		inline Color GetPixel(int x, int y) { return this->buffer[x + y * this->width]; }
	};
}}

#endif  // __SYSTEM_DRAWING_BITMAP_H__
