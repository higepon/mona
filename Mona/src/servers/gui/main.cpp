// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#include <monapi/messages.h>
#include <monapi/cmemoryinfo.h>
#include <gui/messages.h>
#include "GUIServer.h"
#include "image.h"
#include "screen.h"
#include "bzip2.h"
#include "utils.h"

using namespace MonAPI;

static monapi_cmemoryinfo* default_font = NULL;
static ImageInfo* wallpaper = NULL;
static int wallpaper_pos = 0, wallpaper_transparent = -1;
static bool wallpaper_prompt = false;

static void ReadFont(const char* file)
{
    monapi_cmemoryinfo* mi = BZ2DecompressFile(file, true);
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

void DrawWallPaper(const char* src, int pos, int transparent)
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
    DrawWallPaper();
}

void ReadConfig()
{
    monapi_cmemoryinfo* cfg = monapi_call_file_read_data("/MONA.CFG", 0);
    if (cfg == NULL) return;
    
    char line[256], src[256] = "";
    int linepos = 0, wppos = 5, wptp = -1;
    for (dword pos = 0; pos <= cfg->Size; pos++)
    {
        char ch = pos < cfg->Size ? (char)cfg->Data[pos] : '\n';
        if (ch == '\r' || ch == '\n')
        {
            if (linepos > 0)
            {
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
    DrawWallPaper(src, wppos, wptp);
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
                DrawWallPaper(msg.str, msg.arg1, msg.arg2);
                Message::reply(&msg);
                break;
            case MSG_GUISERVER_REFRESHWALLPAPER:
                DrawWallPaper();
                Message::reply(&msg);
                break;
            case MSG_GUISERVER_DECOMPRESSBZ2:
            {
                monapi_cmemoryinfo* mi1 = monapi_cmemoryinfo_new();
                mi1->Handle = msg.arg1;
                mi1->Size   = msg.arg2;
                monapi_cmemoryinfo* mi2 = NULL;
                if (monapi_cmemoryinfo_map(mi1))
                {
                    mi2 = BZ2Decompress(mi1);
                    monapi_cmemoryinfo_dispose(mi1);
                }
                if (mi2 != NULL)
                {
                    Message::reply(&msg, mi2->Handle, mi2->Size);
                    monapi_cmemoryinfo_delete(mi2);
                }
                else
                {
                    Message::reply(&msg);
                }
                monapi_cmemoryinfo_delete(mi1);
                break;
            }
            case MSG_GUISERVER_DECOMPRESSBZ2FILE:
            {
                monapi_cmemoryinfo* mi = BZ2DecompressFile(msg.str, msg.arg1 != 0);
                if (mi != NULL)
                {
                    Message::reply(&msg, mi->Handle, mi->Size);
                    delete mi;
                }
                else
                {
                    Message::reply(&msg);
                }
                break;
            }
        }
    }
}

int MonaMain(List<char*>* pekoe)
{
    ReadConfig();
    ReadFont("/MONA-12.MF2");
    if (default_font == NULL) exit(1);
    
    if (Message::send(Message::lookupMainThread("INIT"), MSG_SERVER_START_OK) != 0)
    {
        printf("%s: INIT error\n", SVR);
        exit(1);
    }

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
