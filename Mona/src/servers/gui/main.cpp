// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#include <monapi/messages.h>
#include <monapi/CString.h>
#include "GUIServer.h"
#include "image.h"
#include "screen.h"
#include "utils.h"

using namespace MonAPI;

static monapi_cmemoryinfo* default_font = NULL;
static guiserver_bitmap* wallpaper = NULL;
static unsigned int background_color = 0;
static bool wallpaper_prompt = false;
static HList<CString>* startup = NULL;

static void ReadFont(const char* file)
{
	monapi_cmemoryinfo* mi = monapi_call_file_decompress_bz2_file(file, true);
	if (mi == NULL) return;
	
	default_font = mi;
}

static void DrawWallPaper(guiserver_bitmap* bmp, int pos, int transparent, const char* src, bool prompt)
{
	if (wallpaper == NULL || bmp == NULL) return;
	
	int x = 0, y = 0, ww = wallpaper->Width, wh = wallpaper->Height;
	switch (pos)
	{
		case 0: // Stretch
		{
			Screen* scr = GetDefaultScreen();
			int sw = scr->getWidth(), sh = scr->getHeight();
			if (sw != ww || sh != wh)
			{
				if (prompt) printf("%s: Resizing %s....", SVR, (const char*)src);
				guiserver_bitmap* bmp2 = ResizeBitmap(bmp, sw, sh);
				MemoryMap::unmap(bmp->Handle);
				bmp = bmp2;
				if (prompt) printf("Done\n");
			}
			break;
		}
		case 1: // Top Left
			break;
		case 2: // Top Center
			x = (ww - bmp->Width) / 2;
			break;
		case 3: // Top Right
			x = ww - bmp->Width;
			break;
		case 4: // Center Left
			y = (wh - bmp->Height) / 2;
			break;
		case 5: // Center Center
			x = (ww - bmp->Width) / 2;
			y = (wh - bmp->Height) / 2;
			break;
		case 6: // Center Right
			x = ww - bmp->Width;
			y = (wh - bmp->Height) / 2;
			break;
		case 7: // Bottom Left
			y = wh - bmp->Height;
			break;
		case 8: // Bottom Center
			x = (ww - bmp->Width) / 2;
			y = wh - bmp->Height;
			break;
		case 9: // Bottom Right
			x = ww - bmp->Width;
			y = wh - bmp->Height;
			break;
	}
	DrawImage(wallpaper, bmp, x, y, -1, -1, -1, -1, transparent);
	MemoryMap::unmap(bmp->Handle);
}

static void DrawWallPaper(const char* src, int pos, int transparent, int background)
{
	bool prompt = wallpaper_prompt;
	wallpaper_prompt = false;
	background_color = background;
	
	Screen* scr = GetDefaultScreen();
	if (wallpaper == NULL)
	{
		wallpaper = CreateBitmap(scr->getWidth(), scr->getHeight(), background_color);
		if (wallpaper == NULL) return;
	}
	else
	{
		FillColor(wallpaper, background_color);
	}
	
	guiserver_bitmap* bmp = ReadImage(src, prompt);
	if (bmp == NULL) return;
	
	DrawWallPaper(bmp, pos, transparent, src, prompt);
	DrawImage(wallpaper);
}

static void ReadConfig()
{
	monapi_cmemoryinfo* cfg = monapi_call_file_read_data("/MONA.CFG", 0);
	if (cfg == NULL) return;
	
	if (startup != NULL)
	{
		delete startup;
		startup = NULL;
	}
	char line[256], src[256] = "";
	int linepos = 0, wppos = 5, wptp = -1, bgcol = background_color;
	for (dword pos = 0; pos <= cfg->Size; pos++)
	{
		char ch = pos < cfg->Size ? (char)cfg->Data[pos] : '\n';
		if (ch == '\r' || ch == '\n')
		{
			if (linepos > 0)
			{
				_A<CString> data = CString(line, linepos).split('=');
				if (data.get_Length() == 2 && data[0] != NULL && data[1] != NULL)
				{
					if (data[0] == "WALLPAPER_SOURCE")
					{
						strncpy(src, data[1], sizeof(src));
						src[sizeof(src) - 1] = '\0';
					}
					else if (data[0] == "WALLPAPER_POSITION")
					{
						wppos = atoi(data[1]);
					}
					else if (data[0] == "WALLPAPER_TRANSPARENT")
					{
						wptp = xtoi(data[1]);
					}
					else if (data[0] == "WALLPAPER_BACKGROUND")
					{
						bgcol = ((unsigned int)xtoi(data[1])) | 0xff000000;
					}
					else if (data[0] == "GUISERVER_STARTUP")
					{
						if (startup == NULL) startup = new HList<CString>();
						startup->add(data[1]);
					}
				}
				linepos = 0;
			}
		}
		else if (linepos < 255)
		{
			line[linepos++] = ch;
		}
	}
	monapi_cmemoryinfo_dispose(cfg);
	monapi_cmemoryinfo_delete(cfg);
	if (src[0] == '\0') return;

	wallpaper_prompt = true;
	DrawWallPaper(src, wppos, wptp, bgcol);
}

static void CheckGUIServer()
{
	syscall_set_ps_dump();
	PsInfo info;

	bool ok = true;
	CString self = "GUI.SVR";
	dword tid = MonAPI::System::getThreadID();

	while (syscall_read_ps_dump(&info) == 0)
	{
		if (ok && self == info.name && tid != info.tid) ok = false;
	}
	if (ok) return;

	printf("%s: already has executed!\n", SVR);
	exit(1);
}

static void MessageLoop()
{
	for (MessageInfo msg;;)
	{
		if (Message::receive(&msg)) continue;
		
		switch (msg.header)
		{
			case MSG_GUISERVER_GETFONT:
				monapi_cmessage_reply_args(&msg, default_font->Handle, default_font->Size, NULL);
				break;
			case MSG_DISPOSE_HANDLE:
				MemoryMap::unmap(msg.arg1);
				monapi_cmessage_reply(&msg);
				break;
			case MSG_GUISERVER_SETWALLPAPER:
				DrawWallPaper(msg.str, msg.arg1, msg.arg2, msg.arg3);
				monapi_cmessage_reply(&msg);
				break;
			case MSG_GUISERVER_DRAWWALLPAPER:
				if (wallpaper != NULL)
				{
					DrawImage(wallpaper, 0, 0, msg.arg1, msg.arg2,
						GET_X_DWORD(msg.arg3), GET_Y_DWORD(msg.arg3));
				}
				monapi_cmessage_reply(&msg);
				break;
			default:
				if (ImageHandler(&msg)) break;
				break;
		}
	}
}

int MonaMain(List<char*>* pekoe)
{
	CheckGUIServer();

	ReadFont("/MONA-12.MF2");
	if (default_font == NULL) exit(1);

	ReadConfig();
	if (startup != NULL)
	{
		for (int i = 0; i < startup->size(); i++)
		{
			monapi_call_elf_execute_file(startup->get(i), 0);
		}
	}

	Message::send(Message::lookupMainThread("INIT"), MSG_SERVER_START_OK);

	MessageLoop();

	monapi_cmemoryinfo_dispose(default_font);
	monapi_cmemoryinfo_delete(default_font);
	if (wallpaper != NULL) MemoryMap::unmap(wallpaper->Handle);
	return 0;
}
