// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#include <jpegls.h>
#include "GUIServer.h"
#include "image.h"
#include "file.h"
#include "bzip2.h"

#define BYTE2INT(array, index) (*(int*)&array[index])

MemoryInfo* ReadBitmap(MemoryInfo* mi)
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
	MemoryInfo* ret = new MemoryInfo();
	if (!ret->Create(w * h * 4))
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

MemoryInfo* ReadJPEG(MemoryInfo* mi)
{
	CJPEGLS* jpeg = new CJPEGLS();
	if (jpeg->Open(mi->Data, mi->Size) != 0)
	{
		delete jpeg;
		return NULL;
	}
	
	int w, h;
	jpeg->GetInfo(&w, &h);
	MemoryInfo* ret = new MemoryInfo();
	if (!ret->Create(w * h * 4))
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

MemoryInfo* ReadImage(const char* file, bool prompt /*= false*/)
{
	MemoryInfo* mi = ReadFile(file, prompt), * ret = NULL;
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
		MemoryInfo* mi2 = BZ2Decompress(mi);
		if (prompt) printf(mi2 != NULL ? "OK\n" : "ERROR\n");
		
		if (mi2 != NULL)
		{
			if (prompt) printf("%s: Decoding %s....", SVR, file);
			ret = ReadBitmap(mi2);
			if (prompt) printf(ret != NULL ? "OK\n" : "ERROR\n");
			mi2->Dispose();
			delete mi2;
		}
	}
	
	mi->Dispose();
	delete mi;
	return ret;
}
