// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

// This file's encoding is UTF-8.

#include <monapi.h>
#include <monalibc.h>
#include <jpegls.h>
#include <gui/messages.h>
using namespace MonAPI;

#define SVR "GUI Server"
#define BYTE2INT(array, index) (*(int*)&array[index])

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
	
	void Dispose()
	{
		MemoryMap::unmap(this->Handle);
	}
};

static MemoryInfo* default_font = NULL;
static MemoryInfo* wallpaper = NULL;
static int wallpaper_pos = 0, wallpaper_transparent = -1;
static bool wallpaper_prompt = false;
static Screen* screen = NULL;
dword msvr = 0;

static MemoryInfo* ReadFile(const char* file, bool prompt = false)
{
	if (prompt) printf("%s: Reading %s....", SVR, file);
	FileInputStream fis(file);
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

static MemoryInfo* ReadImage(const char* file, bool prompt = false)
{
	MemoryInfo* mi = ReadFile(file, prompt), * ret = NULL;
	if (mi == NULL) return ret;
	
	const char* p = &file[strlen(file)];
	for (; *p != '.' && file < p; p--);
	
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

void SetDefaultScreen()
{
	if (screen == NULL) screen = new Screen();
}

void SetMouseCursor(bool enabled)
{
	static bool first = true;
	if (first)
	{
		msvr = Message::lookupMainThread("MOUSE.SVR");
		if (msvr == 0xffffffff)
		{
			printf("%s: Can't connect to mouse server!\n", SVR);
		}
		first = false;
	}
	if (msvr == 0xffffffff) return;
	
	dword hdr = enabled ? MSG_MOUSE_ENABLE_CURSOR : MSG_MOUSE_DISABLE_CURSOR;
	Message::sendReceive(NULL, msvr, hdr);
}

void DrawImage(MemoryInfo* img, int spx, int spy, int ix, int iy, int iw, int ih, int transparent)
{
	SetDefaultScreen();
	SetMouseCursor(false);
	unsigned char* vram = screen->getVRAM();
	int bpp = screen->getBpp(), sw = screen->getWidth(), sh = screen->getHeight();
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
				case 16: // broken
				{
					unsigned short c = pBuf[0] + (pBuf[1] << 8);
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
	SetMouseCursor(true);
}

void DrawWallPaper()
{
	if (wallpaper == NULL) return;
	
	SetDefaultScreen();
	int x = 0, y = 0, sw = screen->getWidth(), sh = screen->getHeight();
	switch (wallpaper_pos)
	{
		case 1: // ¶ã
			break;
		case 2: // ’†‰›ã
			x = (sw - wallpaper->Width) / 2;
			break;
		case 3: // ‰Eã
			x = sw - wallpaper->Width;
			break;
		case 4: // ¶’†‰›
			y = (sh - wallpaper->Height) / 2;
			break;
		case 5: // ’†‰›
			x = (sw - wallpaper->Width) / 2;
			y = (sh - wallpaper->Height) / 2;
			break;
		case 6: // ‰E’†‰›
			x = sw - wallpaper->Width;
			y = (sh - wallpaper->Height) / 2;
			break;
		case 7: // ¶‰º
			y = sh - wallpaper->Height;
			break;
		case 8: // ’†‰›‰º
			x = (sw - wallpaper->Width) / 2;
			y = sh - wallpaper->Height;
			break;
		case 9: // ‰E‰º
			x = sw - wallpaper->Width;
			y = sh - wallpaper->Height;
			break;
	}
	DrawImage(wallpaper, x, y, -1, -1, -1, -1, wallpaper_transparent);
}

void DrawWallPaper(const char* src, int pos, int transparent)
{
	if (wallpaper != NULL) wallpaper->Dispose();
	wallpaper = ReadImage(src, wallpaper_prompt);
	wallpaper_prompt = false;
	if (wallpaper == NULL) return;
	
	wallpaper_pos = pos;
	wallpaper_transparent = transparent;
	DrawWallPaper();
}

int xtoi(const char* str)
{
	if (str == NULL) return 0;
	
	int ret = 0;
	int len = strlen(str);
	for (int i = len - 1, sh = 0; i >= 0; i--, sh += 4)
	{
		char ch = str[i];
		int v = 0;
		if ('0' <= ch && ch <= '9')
		{
			v = ch - '0';
		}
		else if ('A' <= ch && ch <= 'F')
		{
			v = ch - 'A' + 10;
		}
		else if ('a' <= ch && ch <= 'f')
		{
			v = ch - 'a' + 10;
		}
		else
		{
			return 0;
		}
		ret += v << sh;
	}
	return ret;
}

void ReadConfig()
{
	MemoryInfo* cfg = ReadFile("/MONA.CFG");
	if (cfg == NULL) return;
	
    char line[256], src[256] = "";
    int linepos = 0, wppos = 5, wptp = -1;
    for (int pos = 0; pos <= cfg->Size; pos++)
    {
        char ch = pos < cfg->Size ? (char)cfg->Data[pos] : '\n';
        if (ch == '\r' || ch == '\n') {
            if (linepos > 0) {
                line[linepos] = '\0';
                if (strstr(line, "WALLPAPER_SOURCE=") == line)
                {
                	strcpy(src, &line[17]);
                }
                else if (strstr(line, "WALLPAPER_POSITION=") == line)
                {
                	wppos = atoi(&line[19]);
                }
                else if (strstr(line, "WALLPAPER_TRANSPARENT=") == line)
                {
                	wptp = xtoi(&line[22]);
                }
                linepos = 0;
            }
        } else if (linepos < 255) {
            line[linepos++] = ch;
        }
    }
    cfg->Dispose();
    delete cfg;
    if (src[0] == '\0') return;
    
    wallpaper_prompt = true;
    DrawWallPaper(src, wppos, wptp);
}

int MonaMain(List<char*>* pekoe)
{
	ReadConfig();
	ReadFont("/MONA-12.MNF");
	if (default_font == NULL) exit(1);
	
    if (Message::send(Message::lookupMainThread("INIT"), MSG_SERVER_START_OK) != 0)
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
				Message::send(msg.from, MSG_RESULT_OK, msg.header, default_font->Handle, default_font->Size);
				break;
			case MSG_GUISERVER_DECODEIMAGE:
			{
				MemoryInfo* mi = ReadImage(msg.str);
				if (mi != NULL)
				{
					Message::send(msg.from, MSG_RESULT_OK, msg.header, mi->Handle, MAKE_DWORD(mi->Width, mi->Height));
					delete mi;
				}
				else
				{
					Message::send(msg.from, MSG_RESULT_OK, msg.header);
				}
				break;
			}
			case MSG_GUISERVER_DISPOSEIMAGE:
				MemoryMap::unmap(msg.arg1);
				break;
			case MSG_GUISERVER_SETWALLPAPER:
				DrawWallPaper(msg.str, msg.arg1, msg.arg2);
				Message::send(msg.from, MSG_RESULT_OK, msg.header);
				break;
			case MSG_GUISERVER_REFRESHWALLPAPER:
				DrawWallPaper();
				Message::send(msg.from, MSG_RESULT_OK, msg.header);
				break;
		}
	}
	
	default_font->Dispose();
	delete default_font;
	if (wallpaper != NULL)
	{
		wallpaper->Dispose();
		delete wallpaper;
	}
	if (screen != NULL) delete screen;
	return 0;
}
