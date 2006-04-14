// This file is in the public domain.
// There are no restrictions on any sort of usage of this file.

#ifndef __SYSTEM_MONA_FORMS_CONTROLPAINT_H__
#define __SYSTEM_MONA_FORMS_CONTROLPAINT_H__

#include <gui/System/Pointer.h>
#include <gui/System/Drawing/Color.h>
#include <gui/System/Drawing/Graphics.h>
#include <gui/System/Drawing/Rectangle.h>

namespace System { namespace Mona { namespace Forms
{
	class ControlPaint
	{
	public:
		static void DrawRaised(_P<System::Drawing::Graphics> g, int x, int y, int width, int height);
		static void DrawSunken(_P<System::Drawing::Graphics> g, int x, int y, int width, int height);
		static void DrawEngraved(_P<System::Drawing::Graphics> g, int x, int y, int width, int height);
		static void DrawReversibleRectangle(System::Drawing::Rectangle r);
		
		inline static System::Drawing::Color get_LightLight() { return System::Drawing::Color::get_White(); }
		inline static System::Drawing::Color get_Light() { return System::Drawing::Color::FromArgb(0xc0, 0xc0, 0xc0); }
		inline static System::Drawing::Color get_Dark() { return System::Drawing::Color::FromArgb(0x80, 0x80, 0x80); }
		inline static System::Drawing::Color get_DarkDark() { return System::Drawing::Color::get_Black(); }
	};
}}}

#endif  // __SYSTEM_MONA_FORMS_CONTROLPAINT_H__
