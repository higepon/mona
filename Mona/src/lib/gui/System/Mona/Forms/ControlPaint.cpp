// This file is in the public domain.
// There are no restrictions on any sort of usage of this file.

#include <gui/System/Mona/Forms/ControlPaint.h>
#include <gui/System/Drawing/Graphics.h>
#include <gui/System/Math.h>

using namespace System;
using namespace System::Drawing;

#ifdef MONA
extern _P<MonAPI::Screen> GetDefaultScreen();
#else
extern unsigned char* screen_buffer;
extern int screen_width, screen_height;
#ifdef WIN32
extern void MonaGUI_Invalidate();
#endif
#endif

namespace System { namespace Mona { namespace Forms
{
	void ControlPaint::DrawRaised(_P<Graphics> g, int x, int y, int width, int height)
	{
		Color ll = ControlPaint::get_LightLight();
		Color d = ControlPaint::get_Dark();
		Color dd = ControlPaint::get_DarkDark();
		int xw = x + width, yh = y + height;
		
		g->DrawLine(ll, x, y, x, yh - 2);
		g->DrawLine(ll, x, y, xw - 2, y);
		g->DrawLine(dd, x, yh - 1, xw - 1, yh - 1);
		g->DrawLine(dd, xw - 1, y, xw - 1, yh - 1);
		g->DrawLine(d, x + 1, yh - 2, xw - 2, yh - 2);
		g->DrawLine(d, xw - 2, y + 1, xw - 2, yh - 2);
	}
	
	void ControlPaint::DrawSunken(_P<Graphics> g, int x, int y, int width, int height)
	{
		Color ll = ControlPaint::get_LightLight();
		Color l = ControlPaint::get_Light();
		Color d = ControlPaint::get_Dark();
		Color dd = ControlPaint::get_DarkDark();
		int xw = x + width, yh = y + height;
		
		g->DrawLine(dd, x, y, x, yh - 2);
		g->DrawLine(dd, x, y, xw - 2, y);
		g->DrawLine(d, x + 1, y + 1, x + 1, yh - 3);
		g->DrawLine(d, x + 1, y + 1, xw - 3, y + 1);
		g->DrawLine(ll, x, yh - 1, xw - 1, yh - 1);
		g->DrawLine(ll, xw - 1, y, xw - 1, yh - 1);
		g->DrawLine(l, x + 1, yh - 2, xw - 2, yh - 2);
		g->DrawLine(l, xw - 2, y + 1, xw - 2, yh - 2);
	}
	
	void ControlPaint::DrawEngraved(_P<Graphics> g, int x, int y, int width, int height)
	{
		Color ll = ControlPaint::get_LightLight();
		Color dd = ControlPaint::get_DarkDark();
		int xw = x + width, yh = y + height;
		
		g->DrawLine(dd, x, y, xw - 1, y);
		g->DrawLine(dd, x, y, x, yh - 1);
		g->DrawLine(ll, x + 1, y + 1, xw - 1, y + 1);
		g->DrawLine(ll, x + 1, y + 1, x + 1, yh - 1);
		g->DrawLine(ll, xw - 1, yh - 1, xw - 1, y + 2);
		g->DrawLine(ll, xw - 1, yh - 1, x + 2, yh - 1);
		g->DrawLine(dd, xw - 2, yh - 2, xw - 2, y + 2);
		g->DrawLine(dd, xw - 2, yh - 2, x + 2, yh - 2);
	}
	
	void ControlPaint::DrawReversibleRectangle(Rectangle r)
	{
#ifdef MONA
		_P<MonAPI::Screen> scr = ::GetDefaultScreen();
		int w = scr->getWidth(), h = scr->getHeight(), bpp = scr->getBpp();
		unsigned char* vram = scr->getVRAM();
#else
		int w = screen_width, h = screen_height, bpp = 32;
		unsigned char* vram = screen_buffer;
#endif
		int bypp = bpp >> 3;
		int rr = r.get_Right() - 1, rb = r.get_Bottom() - 1;
		int x1 = Math::Max(0, r.X), x2 = Math::Min(w - 1, rr);
		if (0 <= r.Y && r.Y < h)
		{
			unsigned char* pVram = &vram[(x1 + r.Y * w) * bypp];
			int len = (x2 - x1 + 1) * bypp;
			for (int i = 0; i < len; i++, pVram++) *pVram ^= 0xff;
		}
		if (0 <= rb && rb < h)
		{
			unsigned char* pVram = &vram[(x1 + rb * w) * bypp];
			int len = (x2 - x1 + 1) * bypp;
			for (int i = 0; i < len; i++, pVram++) *pVram ^= 0xff;
		}
		int y1 = Math::Max(0, r.Y + 1), y2 = Math::Min(h - 1, rb - 1);
		if (0 <= r.X && r.X < w)
		{
			unsigned char* pVram = &vram[(r.X + y1 * w) * bypp];
			for (int y = y1; y <= y2; y++, pVram += w * bypp)
			{
				for (int i = 0; i < bypp; i++) pVram[i] ^= 0xff;
			}
		}
		if (0 <= rr && rr < w)
		{
			unsigned char* pVram = &vram[(rr + y1 * w) * bypp];
			for (int y = y1; y <= y2; y++, pVram += w * bypp)
			{
				for (int i = 0; i < bypp; i++) pVram[i] ^= 0xff;
			}
		}
#if !defined(MONA) && defined(WIN32)
		MonaGUI_Invalidate();
#endif
	}
}}}
