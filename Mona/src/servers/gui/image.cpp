// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#include <monapi.h>
#include <monapi/messages.h>
#include <jpegls.h>
#include "GUIServer.h"
#include "image.h"

#define BYTE2INT(array, index) (*(int*)&array[index])

using namespace MonAPI;

guiserver_bitmap* CreateBitmap(int width, int height, unsigned int background)
{
	dword handle = MemoryMap::create(sizeof(guiserver_bitmap) + width * height * 4);
	if (handle == 0) return NULL;
	
	guiserver_bitmap* ret = (guiserver_bitmap*)MemoryMap::map(handle);
	if (ret == NULL) return NULL;
	
	ret->Handle = handle;
	ret->Width  = width;
	ret->Height = height;
	
	FillColor(ret, (unsigned int)background);
	
	return ret;
}

guiserver_bitmap* ReadBitmap(monapi_cmemoryinfo* mi)
{
	if (mi->Size < 6) return NULL;
	
	if (mi->Data[0] != 'B' || mi->Data[1] != 'M') return NULL;
	if (mi->Size != (dword)BYTE2INT(mi->Data, 2)) return NULL;
	if (BYTE2INT(mi->Data, 6) != 0) return NULL;
	if (BYTE2INT(mi->Data, 14) != 40) return NULL;
	if (mi->Data[28] + (mi->Data[29] << 8) != 24) return NULL; // 24bpp only
	if (BYTE2INT(mi->Data, 30) != 0) return NULL;
	
	int bfOffBits = BYTE2INT(mi->Data, 10);
	int w = BYTE2INT(mi->Data, 18);
	int h = BYTE2INT(mi->Data, 22);

	dword handle = MemoryMap::create(sizeof(guiserver_bitmap) + w * h * 4);
	if (handle == 0) return NULL;
	
	guiserver_bitmap* ret = (guiserver_bitmap*)MemoryMap::map(handle);
	if (ret == NULL) return NULL;
	
	ret->Handle = handle;
	ret->Width  = w;
	ret->Height = h;
	int p1 = 0;
	int lineSize = (w * 3 + 3) >> 2 << 2;
	for (int y = 0; y < h; y++)
	{
		int p2 = bfOffBits + (h - y - 1) * lineSize;
		for (int x = 0; x < w; x++)
		{
			byte* ptr = (byte*)&ret->Data[p1++];
			ptr[0] = mi->Data[p2++];
			ptr[1] = mi->Data[p2++];
			ptr[2] = mi->Data[p2++];
			ptr[3] = 0xff;
		}
	}
	return ret;
}

guiserver_bitmap* ReadJPEG(monapi_cmemoryinfo* mi)
{
	CJPEGLS* jpeg = new CJPEGLS();
	if (jpeg->Open(mi->Data, mi->Size) != 0)
	{
		delete jpeg;
		return NULL;
	}
	
	int w, h;
	jpeg->GetInfo(&w, &h);

	dword handle = MemoryMap::create(sizeof(guiserver_bitmap) + w * h * 4);
	if (handle == 0)
	{
		delete jpeg;
		return NULL;
	}
	
	guiserver_bitmap* ret = (guiserver_bitmap*)MemoryMap::map(handle);
	if (ret == NULL)
	{
		delete jpeg;
		return NULL;
	}
	
	ret->Handle = handle;
	ret->Width  = w;
	ret->Height = h;
	jpeg->Decode((byte*)ret->Data);
	for (int i = w * h - 1; i >= 0; i--)
	{
		byte* ptr1 = (byte*)&ret->Data[i], * ptr2 = &((byte*)ret->Data)[i * 3];
		ptr1[0] = ptr2[2];
		ptr1[1] = ptr2[1];
		ptr1[2] = ptr2[0];
		ptr1[3] = 0xff;
	}
	delete jpeg;
	return ret;
}

guiserver_bitmap* ReadImage(const CString& file, bool prompt /*= false*/)
{
	CString fn = file.toUpper();
	monapi_cmemoryinfo* mi = monapi_call_file_read_data(fn, prompt);
	guiserver_bitmap* ret = NULL;
	if (mi == NULL) return ret;
	
	if (fn.endsWith(".BMP"))
	{
		if (prompt) printf("%s: Decoding %s....", SVR, (const char*)fn);
		ret = ReadBitmap(mi);
		if (prompt) printf(ret != NULL ? "OK\n" : "ERROR\n");
	}
	else if (fn.endsWith(".JPG"))
	{
		if (prompt) printf("%s: Decoding %s....", SVR, (const char*)fn);
		ret = ReadJPEG(mi);
		if (prompt) printf(ret != NULL ? "OK\n" : "ERROR\n");
	}
	else if (fn.endsWith(".BM2"))
	{
		if (prompt) printf("%s: Decompressing %s....", SVR, (const char*)fn);
		monapi_cmemoryinfo* mi2 = monapi_call_file_decompress_bz2(mi);
		if (prompt) printf(mi2 != NULL ? "OK\n" : "ERROR\n");
		
		if (mi2 != NULL)
		{
			if (prompt) printf("%s: Decoding %s....", SVR, (const char*)fn);
			ret = ReadBitmap(mi2);
			if (prompt) printf(ret != NULL ? "OK\n" : "ERROR\n");
			monapi_cmemoryinfo_dispose(mi2);
			monapi_cmemoryinfo_delete(mi2);
		}
	}

	monapi_cmemoryinfo_dispose(mi);
	monapi_cmemoryinfo_delete(mi);
	return ret;
}

guiserver_bitmap* ResizeBitmap(guiserver_bitmap* bmp, int width, int height)
{
	guiserver_bitmap* ret = CreateBitmap(width, height, 0);
	unsigned char* ptr = (unsigned char*)ret->Data;
	for (int y = 0; y < height; y++)
	{
		double y1 = ((double)(y    )) * ((double)bmp->Height) / ((double)height);
		double y2 = ((double)(y + 1)) * ((double)bmp->Height) / ((double)height);
		for (int x = 0; x < width; x++)
		{
			double x1 = ((double)(x    )) * ((double)bmp->Width) / ((double)width);
			double x2 = ((double)(x + 1)) * ((double)bmp->Width) / ((double)width);
			double size = (x2 - x1) * (y2 - y1);
			double r = 0.0, g = 0.0, b = 0.0;
			for (double y3 = y1, y4; y3 < y2; y3 = y4)
			{
				y4 = (double)(int)(y3 + 1);
				if (y4 > y2) y4 = y2;
				for (double x3 = x1, x4; x3 < x2; x3 = x4)
				{
					x4 = (double)(int)(x3 + 1);
					if (x4 > x2) x4 = x2;
					double size2 = (x4 - x3) * (y4 - y3);
					unsigned char* ptr2 = (unsigned char*)&bmp->Data[((int)x3) + ((int)y3) * bmp->Width];
					b += ((double)ptr2[0]) * size2;
					g += ((double)ptr2[1]) * size2;
					r += ((double)ptr2[2]) * size2;
				}
			}
			ptr[0] = (int)(b / size);
			ptr[1] = (int)(g / size);
			ptr[2] = (int)(r / size);
			ptr[3] = 255;
			ptr += 4;
		}
	}
	return ret;
}

bool ImageHandler(MessageInfo* msg)
{
	switch (msg->header)
	{
		case MSG_GUISERVER_DECODEIMAGE:
		{
			guiserver_bitmap* bmp = ReadImage(msg->str);
			if (bmp != NULL)
			{
				monapi_cmessage_reply_args(msg, bmp->Handle, 0, NULL);
			}
			else
			{
				monapi_cmessage_reply(msg);
			}
			break;
		}
		case MSG_GUISERVER_CREATEBITMAP:
		{
			guiserver_bitmap* bmp = CreateBitmap(msg->arg1, msg->arg2, msg->arg3);
			if (bmp != NULL)
			{
				monapi_cmessage_reply_args(msg, bmp->Handle, 0, NULL);
			}
			else
			{
				monapi_cmessage_reply(msg);
			}
			break;
		}
		default:
			return false;
	}
	return true;
}

void FillColor(guiserver_bitmap* bmp, unsigned int color)
{
	int len = bmp->Width * bmp->Height;
	for (int i = 0; i < len; i++)
	{
		bmp->Data[i] = color;
	}
}

void DrawImage(guiserver_bitmap* dst, guiserver_bitmap* img, int spx, int spy, int ix, int iy, int iw, int ih, int transparent)
{
	int sw = dst->Width, sh = dst->Height;
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
		
		unsigned int* pDst = &dst->Data[(spx + x1 + sy * sw)];
		unsigned int* pImg = &img->Data[(x1 + y * img->Width)];
		for (int x = x1; x < x2; x++, pDst++, pImg++)
		{
			int sx = spx + x;
			if (sx >= sw) break;
			if (sx < 0 || ((*pImg) & 0xff000000) == 0 || transparent == (int)((*pImg) & 0xffffff)) continue;
			
			*pDst = *pImg;
		}
	}
}
