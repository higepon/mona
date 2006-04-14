// This file is in the public domain.
// There are no restrictions on any sort of usage of this file.

#ifndef __SYSTEM_DRAWING_FONT_H__
#define __SYSTEM_DRAWING_FONT_H__

#include <gui/System/Drawing/Color.h>
#include <gui/System/String.h>
#include <gui/System/Pointer.h>
#include <gui/System/Drawing/Size.h>

namespace System { namespace Drawing
{
	class Graphics;
	
	/*internal*/ class FontChar;
	
	class Font : public Object
	{
	private:
		unsigned char* fontData;
		Size minimum, maximum;
		_A<FontChar*> table;
	
	public:
		virtual String get_TypeName() { return "System.Drawing.Font"; }
		Font(unsigned char* fontData);
		virtual ~Font();
	
		void Draw(_P<Graphics> g, String text, Color c, int x, int y);
		void Draw(_P<Graphics> g, wchar ch, Color c, int x, int y);
		Size GetSize(String text);
		Size GetSize(wchar ch);
		
		inline int get_Height() { return this->maximum.Height; }
	
	private:
		FontChar* GetFontChar(wchar ch);
		void Draw(_P<Graphics> g, FontChar* fc, Color c, int x, int y);
	};
}}

#endif  // __SYSTEM_DRAWING_FONT_H__
