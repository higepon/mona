// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#include <monapi.h>
#include <monalibc.h>
#include <jpegls.h>
using namespace MonAPI;

#define SVR "GUI Server"
#define BYTE2INT(array, index) (*(int*)&array[index])

enum
{
	MSG_GUISERVER_GETFONT = 0x4000,
	MSG_GUISERVER_RETURNFONT,
	MSG_GUISERVER_DECODEIMAGE,
	MSG_GUISERVER_RETURNIMAGE,
	MSG_GUISERVER_DISPOSEIMAGE,
	MSG_GUISERVER_SETWALLPAPER
};

static int SendMessage(dword to, dword header, dword arg1, dword arg2, dword arg3)
{
	MessageInfo msg;
	Message::create(&msg, header, arg1, arg2, arg3, NULL);
	return Message::send(to, &msg);
}

class MemoryInfo
{
public:
	dword Handle;
	int Size, Width, Height;
	byte* Data;
	
	MemoryInfo() : Handle(0), Size(0), Width(0), Height(0), Data(NULL) {}
	
	bool Create(int size, bool prompt = false)
	{
		this->Handle = MemoryMap::create(size);
		if (this->Handle == 0)
		{
			if (prompt) printf("ERROR\n");
			printf("%s: MemoryMap create error = %x\n", SVR, MemoryMap::getLastError());
			return false;
		}
		
		this->Data = MemoryMap::map(this->Handle);
		if (this->Data == NULL)
		{
			if (prompt) printf("ERROR\n");
			printf("%s: map error\n", SVR);
			MemoryMap::unmap(this->Handle);
			this->Handle = 0;
			return false;
		}
		
		this->Size = size;
		return true;
	}
};

static MemoryInfo* default_font = NULL;

static MemoryInfo* ReadFile(const char* file, bool prompt = false)
{
	FileInputStream fis((char*)file);
	if (fis.open() != 0)
	{
		if (prompt) printf("ERROR\n");
		return NULL;
	}
	
	MemoryInfo* ret = new MemoryInfo();
	if (!ret->Create(fis.getFileSize(), prompt))
	{
		delete ret;
		return NULL;
	}
	
	fis.read(ret->Data, ret->Size);
	fis.close();
	if (prompt) printf("OK\n");
	return ret;
}

static void ReadFont(const char* file)
{
	printf("%s: Reading %s....", SVR, file);
	MemoryInfo* mi = ReadFile(file, true);
	if (mi == NULL) return;
	
	default_font = mi;
}

static MemoryInfo* ReadBitmap(MemoryInfo* mi)
{
	if (mi->Size < 6) return NULL;
	
	if (mi->Data[0] != 'B' || mi->Data[1] != 'M') return NULL;
	if (mi->Size != BYTE2INT(mi->Data, 2)) return NULL;
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

static MemoryInfo* ReadJPEG(MemoryInfo* mi)
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

static MemoryInfo* ReadImage(const char* file)
{
	MemoryInfo* mi = ReadFile(file), * ret = NULL;
	if (mi == NULL) return ret;
	
	const char* p = file;
	for (; *p != '.' && *p != '\0'; p++);
	
	if (strcmp(p, ".BMP") == 0)
	{
		ret = ReadBitmap(mi);
	}
	else if (strcmp(p, ".JPG") == 0)
	{
		ret = ReadJPEG(mi);
	}
	
	MemoryMap::unmap(mi->Handle);
	delete mi;
	return ret;
}

int MonaMain(List<char*>* pekoe)
{
	ReadFont("/MONA-12.MNF");
	if (default_font == NULL) exit(1);
	
	dword init = Message::lookupMainThread("INIT");
    if (init == 0xFFFFFFFF || SendMessage(init, MSG_SERVER_START_OK, 0, 0, 0) != 0)
	{
		printf("%s: INIT error\n", SVR);
		exit(1);
	}
	
	MessageInfo msg;
	for (;;)
	{
		if (Message::receive(&msg)) continue;
		
		switch (msg.header)
		{
			case MSG_GUISERVER_GETFONT:
				SendMessage(msg.from, MSG_GUISERVER_RETURNFONT, default_font->Handle, default_font->Size, 0);
				break;
			case MSG_GUISERVER_DECODEIMAGE:
			{
				MemoryInfo* mi = ReadImage(msg.str);
				if (mi != NULL)
				{
					SendMessage(msg.from, MSG_GUISERVER_RETURNIMAGE, mi->Handle, mi->Width, mi->Height);
					delete mi;
				}
				else
				{
					SendMessage(msg.from, MSG_GUISERVER_RETURNIMAGE, 0, 0, 0);
				}
				break;
			}
			case MSG_GUISERVER_DISPOSEIMAGE:
				MemoryMap::unmap(msg.arg1);
				break;
			case MSG_GUISERVER_SETWALLPAPER:
				break;
		}
	}
	
	MemoryMap::unmap(default_font->Handle);
	delete default_font;
	return 0;
}
