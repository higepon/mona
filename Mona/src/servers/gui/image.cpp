// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#include <monapi.h>
#include <monapi/messages.h>
#include <jpegls.h>
#include "GUIServer.h"
#include "image.h"

#define BYTE2INT(array, index) (*(int*)&array[index])

ImageInfo::ImageInfo() : Width(0), Height(0)
{
    this->Handle = 0;
    this->Size   = 0;
    this->Data   = NULL;
}

ImageInfo* ReadBitmap(monapi_cmemoryinfo* mi)
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
	ImageInfo* ret = new ImageInfo();
	if (!monapi_cmemoryinfo_create(ret, w * h * 4, 0))
	{
		delete ret;
		return NULL;
	}
	
	ret->Width  = w;
	ret->Height = h;
	int p1 = 0;
	int lineSize = (w * 3 + 3) >> 2 << 2;
	for (int y = 0; y < h; y++)
	{
		int p2 = bfOffBits + (h - y - 1) * lineSize;
		for (int x = 0; x < w; x++)
		{
			ret->Data[p1++] = mi->Data[p2++];
			ret->Data[p1++] = mi->Data[p2++];
			ret->Data[p1++] = mi->Data[p2++];
			ret->Data[p1++] = 0xff;
		}
	}
	return ret;
}

ImageInfo* ReadJPEG(monapi_cmemoryinfo* mi)
{
	CJPEGLS* jpeg = new CJPEGLS();
	if (jpeg->Open(mi->Data, mi->Size) != 0)
	{
		delete jpeg;
		return NULL;
	}
	
	int w, h;
	jpeg->GetInfo(&w, &h);
	ImageInfo* ret = new ImageInfo();
	if (!monapi_cmemoryinfo_create(ret, w * h * 4, 0))
	{
		delete jpeg;
		delete ret;
		return NULL;
	}
	
	ret->Width  = w;
	ret->Height = h;
	jpeg->Decode(ret->Data);
	for (int i = w * h - 1; i >= 0; i--)
	{
		ret->Data[i * 4    ] = ret->Data[i * 3 + 2];
		ret->Data[i * 4 + 1] = ret->Data[i * 3 + 1];
		ret->Data[i * 4 + 2] = ret->Data[i * 3    ];
		ret->Data[i * 4 + 3] = 0xff;
	}
	delete jpeg;
	return ret;
}

ImageInfo* ReadImage(const char* file, bool prompt /*= false*/)
{
	monapi_cmemoryinfo* mi = monapi_call_file_read_data(file, prompt);
    ImageInfo* ret = NULL;
	if (mi == NULL) return ret;
	
	const char* p = &file[strlen(file)];
	for (; *p != '.' && file < p; p--);
	
	if (strcmp(p, ".BMP") == 0)
	{
		if (prompt) printf("%s: Decoding %s....", SVR, file);
		ret = ReadBitmap(mi);
		if (prompt) printf(ret != NULL ? "OK\n" : "ERROR\n");
	}
	else if (strcmp(p, ".JPG") == 0)
	{
		if (prompt) printf("%s: Decoding %s....", SVR, file);
		ret = ReadJPEG(mi);
		if (prompt) printf(ret != NULL ? "OK\n" : "ERROR\n");
	}
	else if (strcmp(p, ".BM2") == 0)
	{
		if (prompt) printf("%s: Decompressing %s....", SVR, file);
		monapi_cmemoryinfo* mi2 = monapi_call_file_decompress_bz2(mi);
		if (prompt) printf(mi2 != NULL ? "OK\n" : "ERROR\n");
		
		if (mi2 != NULL)
		{
			if (prompt) printf("%s: Decoding %s....", SVR, file);
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
