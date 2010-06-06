// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#include "Rectangle.h"
#if __GNUC_MINOR__ < 4
#define _R ::System::Drawing::Rectangle
#else
#define _R System::Drawing::Rectangle
#endif

#include <monapi/messages.h>
#include "image.h"
#include "screen.h"

//using namespace MonAPI;
using MonAPI::Screen;
using MonAPI::Color;

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
		printf("%s: can not allocate buffer!\n", GUI_SERVER_NAME);
		return false;
	}
	
	vram_buffer = CreateBitmap(screen.getWidth(), screen.getHeight(), 0);
	if (vram_buffer == NULL)
	{
		printf("%s: can not allocate VRAM buffer!\n", GUI_SERVER_NAME);
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
    uint64_t end0 = MonAPI::Date::nowInMsec();

	uint8_t* vram = screen.getVRAM();
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
	_R r2(commonParams->mouse.x - 8, commonParams->mouse.y - 8, 32, 32);
	bool mouse = r1.IntersectsWith(r2);
	if (mouse) monapi_call_mouse_set_cursor(MONAPI_FALSE);
      uint64_t end1 = MonAPI::Date::nowInMsec();
      logprintf("GGG :end1 - end0 = %d\n", end1 -end0);
	
	for (int yy = y1; yy < y2; yy++)
	{
		int pos = x1 + yy * sw;
		unsigned int* pSBuf = &screen_buffer->Data[pos];
		unsigned int* pVBuf = &vram_buffer->Data[pos];
		uint8_t* pVram = &vram[pos * bypp];
		for (int xx = x1; xx < x2; xx++, pSBuf++, pVBuf++, pVram += bypp)
		{
			if (*pVBuf == *pSBuf) continue;
			
			*pVBuf = *pSBuf;
			uint8_t* p = (uint8_t*)pSBuf;
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
	
      uint64_t end2 = MonAPI::Date::nowInMsec();
      logprintf("GGG :end2 - end1 = %d\n", end2 -end1);

	if (mouse) monapi_call_mouse_set_cursor(MONAPI_TRUE);
}
