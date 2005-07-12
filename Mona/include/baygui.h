/*
Copyright (c) 2005 bayside

Permission is hereby granted, free of charge, to any person 
obtaining a copy of this software and associated documentation files 
(the "Software"), to deal in the Software without restriction, 
including without limitation the rights to use, copy, modify, merge, 
publish, distribute, sublicense, and/or sell copies of the Software, 
and to permit persons to whom the Software is furnished to do so, 
subject to the following conditions:

The above copyright notice and this permission notice shall be 
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY 
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#if !defined(_BAYGUI_H_INCLUDED_)
#define _BAYGUI_H_INCLUDED_

// ==================================================
// OS 定義部分
// ==================================================
#define MONA  1
//#define SDL 1
//#define OSASK 1

// ==================================================
// OS 依存ヘッダファイル
// ==================================================

typedef unsigned int dword;
typedef unsigned int wchar;

#if defined(MONA)
#include <monapi.h>
#include <monapi/messages.h>
#include <monalibc.h>
#include <gui/messages.h>
#elif defined(SDL)
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

// ==================================================
// SDL依存部分
// ==================================================

#include <SDL.h>
#undef   WIN32
#undef   main

/** スクリーンオブジェクト */
extern SDL_Surface* screen;

// ==================================================
// GUIサーバ依存部分
// ==================================================

/** GUIサーバー内ビットマップ構造体 */
typedef struct {
	dword Handle;
	int Width, Height;
	dword* Data;
} guiserver_bitmap;

/** GUIサーバー内ウィンドウ（部品）構造体 */
typedef struct {
	dword Handle, Parent, Owner, ThreadID;
	int X, Y, Width, Height, OffsetX, OffsetY, Opacity;
	bool Visible, Focused;
	dword Flags, TransparencyKey;
	dword BufferHandle, FormBufferHandle;
	guiserver_bitmap* __internal1;
	bool __internal2;
	char __reserved[64];
} guiserver_window;

/** GUIサーバー内ウィンドウスタイル */
enum
{
	WINDOWFLAGS_NOBORDER = 1,
	WINDOWFLAGS_MODAL = 2,
	WINDOWFLAGS_NOACTIVATE = 4,
	WINDOWFLAGS_TOPMOST = 8,
	WINDOWFLAGS_BOTTOMMOST = 16
};

#define MAKE_DWORD(a, b) (((dword)(unsigned short)(a)) + (((dword)(unsigned short)(b)) << 16))
#define GET_X_DWORD(a) ((int)(short)(a & 0xffff))
#define GET_Y_DWORD(a) ((int)(short)((a) >> 16))

// ==================================================
// MonAPI依存部分
// ==================================================

/** MonAPIダミー定数 */
enum {
	MONAPI_FALSE = 0,
	MONAPI_TRUE  = 1,
	THREAD_UNKNOWN,
	ID_GUI_SERVER,
	MSG_GUISERVER_CREATEWINDOW,
	MSG_GUISERVER_CREATEBITMAP,
	MSG_GUISERVER_ACTIVATEWINDOW,
	MSG_GUISERVER_DISPOSEWINDOW,
	MSG_GUISERVER_MOVEWINDOW,
	MSG_GUISERVER_DRAWWINDOW,
	MSG_GUISERVER_MOUSECAPTURE,
	MSG_GUISERVER_CREATEOVERLAP,
	MSG_GUISERVER_DISPOSEOVERLAP,
	MSG_GUISERVER_MOVEOVERLAP,
	MSG_GUISERVER_DECODEIMAGE,
	MSG_GUISERVER_DISPOSEBITMAP
};

/** MonAPIダミー構造体 */
typedef struct {
	dword header;
	dword arg1;
	dword arg2;
	dword arg3;
	dword from;
	char str[128];
	int length;
} MessageInfo;

namespace MonAPI {
	/** MonAPIダミークラス */
	class MemoryMap {
	public:
		static void* map(dword size)
		{
			char* memory = new char [size];
			memset(memory, 0, size);
			return memory;
		}
	};
	
	/** MonAPIダミークラス */
	class Message {
	public:
		static int send(dword thread_id, dword msg_id, dword arg1)
		{
			return 0;
		}
		
		static int sendReceive(MessageInfo* info, dword thread_id, dword msg_id)
		{
			if (msg_id == MSG_GUISERVER_CREATEWINDOW) {
				info->arg2 = sizeof(guiserver_window);
			}
			return 0;
		}
		
		static int sendReceive(MessageInfo* info, dword thread_id, dword msg_id, dword arg1)
		{
			return 0;
		}
		
		static int sendReceive(MessageInfo* info, dword thread_id, dword msg_id, dword arg1, dword arg2)
		{
			return 0;
		}
		
		static int sendReceive(MessageInfo* info, dword thread_id, dword msg_id, dword arg1, dword arg2, dword arg3)
		{
			if (msg_id == MSG_GUISERVER_CREATEBITMAP) {
				info->arg2 = sizeof(guiserver_bitmap);
			}
			return 0;
		}
		
		static int sendReceive(MessageInfo* info, dword thread_id, dword msg_id, dword arg1, dword arg2, dword arg3, const char* str)
		{
			return 0;
		}
	};
	
	/** MonAPIダミークラス */
	class System {
	public:
		static dword getThreadID()
		{
			return 0;
		}
	};
}

/** MonAPIダミーメソッド */
dword monapi_get_server_thread_id(dword serverID);

/** MonAPIダミーメソッド */
bool monapi_register_to_server(int serverID, int isCout);

/** MonAPIダミーメソッド */
void kill_timer(dword timerID);
#endif

// ==================================================
// BayGUI グローバルマクロ
// ==================================================

#define inGetUInt16(b) (unsigned short)( (unsigned short)((b)[1])<<8 | (unsigned short)((b)[0]) )
#define inGetUInt32(b) (dword)( (dword)((b)[3])<<24 | (dword)((b)[2])<<16 | (dword)((b)[1])<<8 | (dword)((b)[0]) )
#define inGetUInt16(b) (unsigned short)( (unsigned short)((b)[1])<<8 | (unsigned short)((b)[0]) )

#define BAYGUI_VERSION "BayGUI $Date$"
#define BAYGUI_COPYING "Copyright (C) 2004-2005, bayside."
//#define WINDOWMANAGER_MENU_TITLE_JP "ファイル　編集　表示　特別　ヘルプ"
//#define WINDOWMANAGER_MENU_TITLE_EN "File Edit View Special Help"
//#define WINDOWMANAGER_STARUP_MESSAGE "starting baygui ...\n"
//#define WINDOWMANAGER_SHUTDOWN_MESSAGE "shutdown baygui ...\n"

// ==================================================
// BayGUI ヘッダファイル
// ==================================================
#include "baygui/Font.h"
#include "baygui/Color.h"
#include "baygui/Object.h"
#include "baygui/Pointer.h"
#include "baygui/String.h"
#include "baygui/Vector.h"
#include "baygui/Hashtable.h"
#include "baygui/Event.h"
#include "baygui/KeyEvent.h"
#include "baygui/MouseEvent.h"
#include "baygui/insets.h"
#include "baygui/Rectangle.h"
#include "baygui/FontMetrics.h"
#include "baygui/Image.h"
#include "baygui/Graphics.h"
#include "baygui/Component.h"
#include "baygui/Checkbox.h"
#include "baygui/CheckboxGroup.h"
#include "baygui/Button.h"
#include "baygui/Label.h"
#include "baygui/ListBox.h"
#include "baygui/Scrollbar.h"
#include "baygui/TextField.h"
#include "baygui/Container.h"
#include "baygui/Window.h"
#include "baygui/Frame.h"

using namespace baygui;

#endif // _BAYGUI_H_INCLUDED_
