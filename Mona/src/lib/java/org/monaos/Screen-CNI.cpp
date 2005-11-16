// This file was created by `gcjh -stubs'. -*- c++ -*-
//
// This file is intended to give you a head start on implementing native
// methods using CNI.
// Be aware: running `gcjh -stubs ' once more for this class may
// overwrite any edits you have made to this file.

#include <org/monaos/Screen.h>
#include <gcj/cni.h>
#ifdef MONA
#include <monapi.h>
#endif

jint
org::monaos::Screen::getWidth ()
{
#ifdef MONA
	volatile ScreenInfo info;
	syscall_get_vram_info(&info);
	return info.x;
#else
	return 0;
#endif
}


jint
org::monaos::Screen::getHeight ()
{
#ifdef MONA
	volatile ScreenInfo info;
	syscall_get_vram_info(&info);
	return info.y;
#else
	return 0;
#endif
}


jint
org::monaos::Screen::getBpp ()
{
#ifdef MONA
	volatile ScreenInfo info;
	syscall_get_vram_info(&info);
	return info.bpp;
#else
	return 0;
#endif
}


jint
org::monaos::Screen::getPixel (jint x, jint y)
{
#ifdef MONA
	volatile ScreenInfo info;
	syscall_get_vram_info(&info);
	if (x < 0 || y < 0 || x >= (jint)info.x || y >= (jint)info.y) return 0;
	byte* _vram = (byte*)info.vram;
	int bpp = info.bpp >> 3;
	byte* vramPtr  = &_vram[(x + y * info.x) * bpp];

	if (bpp == 1) {
		return 0;
	} else if (bpp == 2) {
		word color = (word)(vramPtr[0] | (vramPtr[1] << 8));
		return (jint)((color & 0x001f) << 3 | ((color & 0x07e0) << 5) | ((color & 0xf800) << 8) | 0x070307);
	} else {
		return (jint)(vramPtr[2] | (vramPtr[1]  <<  8) | (vramPtr[0]  << 16));
	}
#else
	return 0;
#endif
}


void
org::monaos::Screen::putPixel (jint x, jint y, jint color)
{
#ifdef MONA
	volatile ScreenInfo info;
	syscall_get_vram_info(&info);
	if (x < 0 || y < 0 || x >= (jint)info.x || y >= (jint)info.y) return;
	byte* _vram = (byte*)info.vram;
	int bpp = info.bpp >> 3;
	byte* vramPtr  = &_vram[(x + y * info.x) * bpp];

	if (bpp == 1) {
		// do nothing
	} else if (bpp == 2) {
		*((word*)vramPtr) = (word)(((color >> 8) & 0xF800) | ((color >> 5) & 0x07E0) | ((color >> 3) & 0x001F));
	} else {
		byte* colorPtr = (byte*)&color;
		vramPtr[0] = colorPtr[0];
		vramPtr[1] = colorPtr[1];
		vramPtr[2] = colorPtr[2];
	}
#else
#endif
}


void
org::monaos::Screen::bitblt (jint x, jint y, jint width, jint height, jintArray image)
{
#ifdef MONA
	volatile ScreenInfo info;
	syscall_get_vram_info(&info);
	if (x < 0 || y < 0 || (x + width) >= (jint)info.x || (y + height) >= (jint)info.y) return;
	byte* _vram = (byte*)info.vram;
	int* _color = (int*)elements(image);
	int bpp = info.bpp >> 3;
	
	if (bpp == 1) {
		// do nothing
	} else if (bpp == 2) {
		for (int y0 = 0; y0 < height; y0++) {
			for (int x0 = 0; x0 < width; x0++) {
				byte* vramPtr  = &_vram[((x + x0) + (y + y0) * info.x) * bpp];
				int color = _color[y0 * width + x0];
				*((word*)vramPtr) = (word)(((color >> 8) & 0xF800) | ((color >> 5) & 0x07E0) | ((color >> 3) & 0x001F));
			}
		}
	} else {
		for (int y0 = 0; y0 < height; y0++) {
			for (int x0 = 0; x0 < width; x0++) {
				byte* vramPtr  = &_vram[((x + x0) + (y + y0) * info.x) * bpp];
				byte* colorPtr = (byte*)_color[y0 * width + x0];
				vramPtr[0] = colorPtr[0];
				vramPtr[1] = colorPtr[1];
				vramPtr[2] = colorPtr[2];
			}
		}
	}
#else
#endif
}


