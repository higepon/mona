// This file is in the public domain.
// There are no restrictions on any sort of usage of this file.

#ifndef __SYSTEM_DRAWING_GRAPHICS_H__
#define __SYSTEM_DRAWING_GRAPHICS_H__

#include <gui/System/Pointer.h>
#include <gui/System/Drawing/Rectangle.h>
#include <gui/System/Drawing/Bitmap.h>

namespace System { namespace Drawing
{
	class Font;
	
	class Graphics : public Object
	{
	private:
		_P<Bitmap> image;
		Rectangle clientRectangle;
	
	private:
		Graphics(_P<Bitmap> image);
	
	public:
		virtual String get_TypeName() { return "System.Drawing.Graphics"; }
		static _P<Graphics> FromImage(_P<Bitmap> image);
		virtual ~Graphics();
		void Dispose();
		
		void SetPixel(int x, int y, Color c);
		void DrawLine(Color c, int x1, int y1, int x2, int y2);
		void DrawRectangle(Color c, int x, int y, int width, int height);
		void FillRectangle(Color c, int x, int y, int width, int height);
		void DrawEllipse(Color c, int x, int y, int width, int height);
		void FillEllipse(Color c, int x, int y, int width, int height);
		void DrawString(String text, _P<Font> font, Color c, int x, int y);
		Size MeasureString(String text, _P<Font> font);
		void DrawImage(_P<Bitmap> image, int x, int y);
		
		inline Point get_Offset() { return this->clientRectangle.get_Location(); }
		inline void set_ClientRectangle(Rectangle r) { this->clientRectangle = r; }
	};
}}

#endif  // __SYSTEM_DRAWING_GRAPHICS_H__
