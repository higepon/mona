// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#include <gui/System/Drawing/Rectangle.h>
#define _R System::Drawing::Rectangle

#include <monapi/messages.h>
#include "GUIServer.h"
#include "image.h"
#include "screen.h"

using namespace MonAPI;

extern CommonParameters* commonParams;

guiserver_bitmap* screen_buffer, * vram_buffer;
static Screen screen;

Screen* GetDefaultScreen()
{
	return &screen;
}

bool InitScreen()
{
	screen_buffer = CreateBitmap(screen.getWidth(), screen.getHeight(), 0);
	if (screen_buffer == NULL)
	{
		printf("%s: can not allocate buffer!\n", SVR);
		return false;
	}
	
	vram_buffer = CreateBitmap(screen.getWidth(), screen.getHeight(), 0);
	if (vram_buffer == NULL)
	{
		printf("%s: can not allocate VRAM buffer!\n", SVR);
		return false;
	}
	
	return true;
}

void DisposeScreen()
{
	DisposeBitmap(screen_buffer->Handle);
	DisposeBitmap(vram_buffer->Handle);
}

void DrawScreen(int x /*= 0*/, int y /*= 0*/, int w /*= -1*/, int h /*= -1*/)
{
	byte* vram = screen.getVRAM();
	int bpp = screen.getBpp(), sw = screen.getWidth(), sh = screen.getHeight();
	int bypp = bpp >> 3;
	if (w < 0) w = sw;
	if (h < 0) h = sh;
	int x1 = x, y1 = y, x2 = x + w, y2 = y + h;
	if (x1 < 0) x1 = 0;
	if (y1 < 0) y1 = 0;
	if (x2 > sw) x2 = sw;
	if (y2 > sh) y2 = sh;
	
	_R r1(x1, y1, x2 - x1 + 1, y2 - y1 + 1);
	_R r2(commonParams->mouse.x - 4, commonParams->mouse.y - 4, 16, 16);
	bool mouse = r1.IntersectsWith(r2);
	if (mouse) monapi_call_mouse_set_cursor(MONAPI_FALSE);
	
	for (int yy = y1; yy < y2; yy++)
	{
		int pos = x1 + yy * sw;
		unsigned int* pSBuf = &screen_buffer->Data[pos];
		unsigned int* pVBuf = &vram_buffer->Data[pos];
		byte* pVram = &vram[pos * bypp];
		for (int xx = x1; xx < x2; xx++, pSBuf++, pVBuf++, pVram += bypp)
		{
			if (*pVBuf == *pSBuf) continue;
			
			*pVBuf = *pSBuf;
			byte* p = (byte*)pSBuf;
			switch (bpp)
			{
				case 8: // broken
					*pVram = (p[0] + p[1] + p[2]) / 3;
					break;
				case 16: // 565
					*(unsigned short*)pVram = Color::bpp24to565(p);
					break;
				case 24:
					pVram[0] = p[0];
					pVram[1] = p[1];
					pVram[2] = p[2];
					break;
				case 32:
					*(unsigned int*)pVram = *pSBuf;
					break;
			}
		}
	}
	
	if (mouse) monapi_call_mouse_set_cursor(MONAPI_TRUE);
}
