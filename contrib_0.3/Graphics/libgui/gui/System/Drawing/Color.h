// This file is in the public domain.
// There are no restrictions on any sort of usage of this file.

#ifndef __SYSTEM_DRAWING_COLOR_H__
#define __SYSTEM_DRAWING_COLOR_H__

namespace System { namespace Drawing
{
	struct Color
	{
	private:
		int argb;
	
	public:
		inline static Color FromArgb(int v)
		{
			Color ret;
			ret.argb = v;
			return ret;
		}
		
		inline static Color FromArgb(int a, int r, int g, int b)
		{
			return Color::FromArgb((a << 24) | (r << 16) | (g << 8) | b);
		}
		
		inline static Color FromArgb(int r, int g, int b)
		{
			return Color::FromArgb(0xff, r, g, b);
		}
		
		inline Color& operator =(const Color& c)
		{
			this->argb = c.argb;
			return *this;
		}
		
		inline bool operator ==(const Color& c) { return this->argb == c.argb; }
		inline bool operator !=(const Color& c) { return this->argb != c.argb; }
		
		inline int ToArgb() { return this->argb; }
		inline int get_A() { return ((unsigned int)this->argb) >> 24; }
		inline int get_R() { return this->argb >> 16 & 0xff; }
		inline int get_G() { return this->argb >>  8 & 0xff; }
		inline int get_B() { return this->argb       & 0xff; }
		
		inline static Color get_Empty  () { return Color::FromArgb(0,  0,  0,  0); }
		inline static Color get_Black  () { return Color::FromArgb(  0,   0,   0); }
		inline static Color get_Blue   () { return Color::FromArgb(  0,   0, 255); }
		inline static Color get_Red    () { return Color::FromArgb(255,   0,   0); }
		inline static Color get_Magenta() { return Color::FromArgb(255,   0, 255); }
		inline static Color get_Green  () { return Color::FromArgb(  0, 255,   0); }
		inline static Color get_Cyan   () { return Color::FromArgb(  0, 255, 255); }
		inline static Color get_Yellow () { return Color::FromArgb(255, 255,   0); }
		inline static Color get_White  () { return Color::FromArgb(255, 255, 255); }
	};
}}

#endif  // __SYSTEM_DRAWING_COLOR_H__
