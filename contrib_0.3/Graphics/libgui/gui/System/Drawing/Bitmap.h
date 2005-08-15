// This file is in the public domain.
// There are no restrictions on any sort of usage of this file.

#ifndef __SYSTEM_DRAWING_BITMAP_H__
#define __SYSTEM_DRAWING_BITMAP_H__

#include <servers/gui.h>
#include <gui/System/Object.h>
#include <monapi/Array.h>
#include <gui/System/Drawing/Color.h>

namespace System { namespace Drawing
{
	class Bitmap : public System::Object
	{
	protected:
		int width, height;
		_A<Color> buffer;
		guiserver_bitmap* _object;
	
	public:
		virtual String get_TypeName() { return "System.Drawing.Bitmap"; }
		Bitmap(int width, int height);
		Bitmap(String fileName);
		virtual ~Bitmap();
		
		inline unsigned int get_Handle() { return this->_object->Handle; }
		virtual void Dispose();
		inline int get_Width() { return this->width; }
		inline int get_Height() { return this->height; }
		inline Color& operator [](int index) { return this->buffer[index]; }
		inline Color* get() { return this->buffer.get(); }
	
		void SetPixel(int x, int y, Color c);
		inline Color GetPixel(int x, int y)
		{
			return (x < 0 || this->width <= x || y < 0 || this->height <= y)
				? Color::get_Empty() : this->buffer[x + y * this->width];
		}
	};
}}

#endif  // __SYSTEM_DRAWING_BITMAP_H__
