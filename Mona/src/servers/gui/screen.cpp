// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#include <monapi/messages.h>
#include "GUIServer.h"
#include "screen.h"
using namespace MonAPI;

static Screen screen;

Screen* GetDefaultScreen()
{
	return &screen;
}

void DrawImage(ImageInfo* img, int spx, int spy, int ix, int iy, int iw, int ih, int transparent)
{
	monapi_call_mouse_set_cursor(0);
	unsigned char* vram = screen.getVRAM();
	int bpp = screen.getBpp(), sw = screen.getWidth(), sh = screen.getHeight();
	int bypp = bpp >> 3;
	if (ix < 0) ix = 0;
	if (iy < 0) iy = 0;
	if (iw < 0) iw = img->Width;
	if (ih < 0) ih = img->Height;
	int x1 = ix, y1 = iy, x2 = ix + iw, y2 = iy + ih;
	if (x2 > img->Width ) x2 = img->Width;
	if (y2 > img->Height) y2 = img->Height;
	for (int y = y1; y < y2; y++)
	{
		int sy = spy + y;
		if (sy >= sh) break;
		if (sy < 0) continue;
		
		byte* pBuf = &img->Data[(x1 + y * img->Width) * 4];
		unsigned char* pVram = &vram[(spx + x1 + sy * sw) * bypp];
		for (int x = x1; x < x2; x++, pVram += bypp, pBuf += 4)
		{
			int sx = spx + x;
			if (sx >= sw) break;
			int c32 = (int)((*(unsigned int*)pBuf) & 0xffffff);
			if (sx < 0 || pBuf[3] == 0 || c32 == transparent) continue;
			
			switch (bpp)
			{
				case 8: // broken
				{
					byte c = (pBuf[0] + pBuf[1] + pBuf[2]) / 3;
					if (*pVram != c) *pVram = c;
					break;
				}
				case 16: // 565
				{
					unsigned short c = Color::bpp24to565(pBuf);
					if (*(unsigned short*)pVram != c) *(unsigned short*)pVram = c;
					break;
				}
				case 24:
					if (pVram[0] != pBuf[0]) pVram[0] = pBuf[0];
					if (pVram[1] != pBuf[1]) pVram[1] = pBuf[1];
					if (pVram[2] != pBuf[2]) pVram[2] = pBuf[2];
					break;
				case 32:
					if (*(int*)pVram != c32) *(int*)pVram = c32;
					break;
			}
		}
	}
	monapi_call_mouse_set_cursor(1);
}
