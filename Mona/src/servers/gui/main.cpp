// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#include <monapi/messages.h>
#include <monapi/cmemoryinfo.h>
#include <monapi/CString.h>
#include <gui/messages.h>
#include "GUIServer.h"
#include "image.h"
#include "screen.h"
#include "utils.h"

using namespace MonAPI;

static monapi_cmemoryinfo* default_font = NULL;
static ImageInfo* wallpaper = NULL;
static int wallpaper_pos = 0, wallpaper_transparent = -1, background_color = 0xffffff;
static bool wallpaper_prompt = false;
static HList<CString>* startup = NULL;

static void ReadFont(const char* file)
{
	monapi_cmemoryinfo* mi = monapi_call_file_decompress_bz2_file(file, true);
	if (mi == NULL) return;
	
	default_font = mi;
}

void DrawWallPaper()
{
	if (wallpaper == NULL) return;
	
	Screen* scr = GetDefaultScreen();
	int x = 0, y = 0, sw = scr->getWidth(), sh = scr->getHeight();
	switch (wallpaper_pos)
	{
		case 1: // Top Left
			break;
		case 2: // Top Center
			x = (sw - wallpaper->Width) / 2;
			break;
		case 3: // Top Right
			x = sw - wallpaper->Width;
			break;
		case 4: // Center Left
			y = (sh - wallpaper->Height) / 2;
			break;
		case 5: // Center Center
			x = (sw - wallpaper->Width) / 2;
			y = (sh - wallpaper->Height) / 2;
			break;
		case 6: // Center Right
			x = sw - wallpaper->Width;
			y = (sh - wallpaper->Height) / 2;
			break;
		case 7: // Bottom Left
			y = sh - wallpaper->Height;
			break;
		case 8: // Bottom Center
			x = (sw - wallpaper->Width) / 2;
			y = sh - wallpaper->Height;
			break;
		case 9: // Bottom Right
			x = sw - wallpaper->Width;
			y = sh - wallpaper->Height;
			break;
	}
	DrawImage(wallpaper, x, y, -1, -1, -1, -1, wallpaper_transparent);
}

void DrawWallPaper(const char* src, int pos, int transparent, int background)
{
	if (wallpaper != NULL)
	{
		monapi_cmemoryinfo_dispose(wallpaper);
		monapi_cmemoryinfo_delete(wallpaper);
	}
	wallpaper = ReadImage(src, wallpaper_prompt);
	wallpaper_prompt = false;
	if (wallpaper == NULL) return;
	
	wallpaper_pos = pos;
	wallpaper_transparent = transparent;
	if (background != -1) background_color = background;
	DrawWallPaper();
}

void ReadConfig()
{
	monapi_cmemoryinfo* cfg = monapi_call_file_read_data("/MONA.CFG", 0);
	if (cfg == NULL) return;
	
	if (startup != NULL)
	{
		delete startup;
		startup = NULL;
	}
	char line[256], src[256] = "";
	int linepos = 0, wppos = 5, wptp = -1, bgcol = -1;
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
						bgcol = xtoi(data[1]);
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

void MessageLoop()
{
	for (MessageInfo msg;;)
	{
		if (Message::receive(&msg)) continue;
		
		switch (msg.header)
		{
			case MSG_GUISERVER_GETFONT:
				Message::reply(&msg, default_font->Handle, default_font->Size);
				break;
			case MSG_GUISERVER_DECODEIMAGE:
			{
				ImageInfo* mi = ReadImage(msg.str);
				if (mi != NULL)
				{
					Message::reply(&msg, mi->Handle, MAKE_DWORD(mi->Width, mi->Height));
					delete mi;
				}
				else
				{
					Message::reply(&msg);
				}
				break;
			}
			case MSG_DISPOSE_HANDLE:
				MemoryMap::unmap(msg.arg1);
				Message::reply(&msg);
				break;
			case MSG_GUISERVER_SETWALLPAPER:
				DrawWallPaper(msg.str, msg.arg1, msg.arg2, msg.arg3);
				Message::reply(&msg);
				break;
			case MSG_GUISERVER_REFRESHWALLPAPER:
				DrawWallPaper();
				Message::reply(&msg);
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
	if (wallpaper != NULL)
	{
		monapi_cmemoryinfo_dispose(wallpaper);
		monapi_cmemoryinfo_delete(wallpaper);
	}
	return 0;
}
