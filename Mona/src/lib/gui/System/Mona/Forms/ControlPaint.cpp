// This file is in the public domain.
// There are no restrictions on any sort of usage of this file.

#include <gui/System/Mona/Forms/ControlPaint.h>
#include <gui/System/Drawing/Graphics.h>
#include <gui/System/Math.h>

using namespace System;
using namespace System::Drawing;

extern _P<MonAPI::Screen> GetDefaultScreen();

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
		_P<MonAPI::Screen> scr = ::GetDefaultScreen();
		int w = scr->getWidth(), h = scr->getHeight(), bpp = scr->getBpp(), bypp = bpp >> 3;
		int rr = r.get_Right() - 1, rb = r.get_Bottom() - 1;
		byte* vram = scr->getVRAM();
		int x1 = Math::Max(0, r.X), x2 = Math::Min(w - 1, rr);
		if (0 <= r.Y && r.Y < h)
		{
			byte* pVram = &vram[(x1 + r.Y * w) * bypp];
			int len = (x2 - x1 + 1) * bypp;
			for (int i = 0; i < len; i++, pVram++) *pVram ^= 0xff;
		}
		if (0 <= rb && rb < h)
		{
			byte* pVram = &vram[(x1 + rb * w) * bypp];
			int len = (x2 - x1 + 1) * bypp;
			for (int i = 0; i < len; i++, pVram++) *pVram ^= 0xff;
		}
		int y1 = Math::Max(0, r.Y + 1), y2 = Math::Min(h - 1, rb - 1);
		if (0 <= r.X && r.X < w)
		{
			byte* pVram = &vram[(r.X + y1 * w) * bypp];
			for (int y = y1; y <= y2; y++, pVram += w * bypp)
			{
				for (int i = 0; i < bypp; i++) pVram[i] ^= 0xff;
			}
		}
		if (0 <= rr && rr < w)
		{
			byte* pVram = &vram[(rr + y1 * w) * bypp];
			for (int y = y1; y <= y2; y++, pVram += w * bypp)
			{
				for (int i = 0; i < bypp; i++) pVram[i] ^= 0xff;
			}
		}
	}
}}}
