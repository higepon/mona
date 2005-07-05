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
	#include <SDL.h>
	#undef   WIN32
	#undef   main

	// スクリーンオブジェクト
	extern SDL_Surface* screen;

	// GUIサーバー内ウィンドウスタイル
	enum
	{
		WINDOWFLAGS_NOBORDER = 1,
		WINDOWFLAGS_MODAL = 2,
		WINDOWFLAGS_NOACTIVATE = 4,
		WINDOWFLAGS_TOPMOST = 8,
		WINDOWFLAGS_BOTTOMMOST = 16
	};

	// GUIサーバー内ビットマップ構造体
	typedef struct {
		unsigned int Handle;
		int Width, Height;
		unsigned int* Data;
	} guiserver_bitmap;

	// GUIサーバー内ウィンドウ（部品）構造体
	typedef struct {
		unsigned int Handle, Parent, Owner, ThreadID;
		int X, Y, Width, Height, OffsetX, OffsetY, Opacity;
		bool Visible, Focused;
		unsigned int Flags, TransparencyKey;
		unsigned int BufferHandle, FormBufferHandle;
		guiserver_bitmap* __internal1;
		bool __internal2;
		char __reserved[64];
	} guiserver_window;
#else
	// とりあえずコンパイルが通るだけ
	#define NULL 0
	int strcmp(const char*, const char*) { return 0; }
	unsigned long strlen(const char*) { return 0; }
	char* strcpy(char*, const char*) { return ""; }
	void* memcpy(void* s1, const void* s2, unsigned long size) { return NULL; }
	void* memset(void*, int, unsigned long) { return NULL; }
#endif

// ==================================================
// BayGUI グローバルマクロ
// ==================================================
#define inGetUInt16(b) (unsigned short)( (unsigned short)((b)[1])<<8 | (unsigned short)((b)[0]) )
#define inGetUInt32(b) (unsigned int)( (unsigned int)((b)[3])<<24 | (unsigned int)((b)[2])<<16 | (unsigned int)((b)[1])<<8 | (unsigned int)((b)[0]) )
#define inGetUInt16(b) (unsigned short)( (unsigned short)((b)[1])<<8 | (unsigned short)((b)[0]) )

#define BAYGUI_VERSION "BayGUI $Date$"
#define BAYGUI_COPYING "Copyright (C) 2004-2005, bayside."
//#define WINDOWMANAGER_MENU_TITLE_JP "ファイル　編集　表示　特別　ヘルプ"
//#define WINDOWMANAGER_MENU_TITLE_EN "File Edit View Special Help"
//#define WINDOWMANAGER_STARUP_MESSAGE "starting baygui ...\n"
//#define WINDOWMANAGER_SHUTDOWN_MESSAGE "shutdown baygui ...\n"

/** テキストフィールド文字列最大長 */
#define MAX_TEXT_LEN      128

// ==================================================
// BayGUI ヘッダファイル
// ==================================================
#include "baygui/Font.h"
#include "baygui/Color.h"
#include "baygui/Object.h"
#include "baygui/Pointer.h"
#include "baygui/String.h"
#include "baygui/LinkedList.h"
#include "baygui/Vector.h"
#include "baygui/Hashtable.h"
#include "baygui/Event.h"
#include "baygui/KeyEvent.h"
#include "baygui/MouseEvent.h"
#include "baygui/Rect.h"
#include "baygui/FontMetrics.h"
#include "baygui/Image.h"
#include "baygui/Graphics.h"
#include "baygui/Control.h"
#include "baygui/Checkbox.h"
#include "baygui/CheckboxGroup.h"
#include "baygui/Button.h"
#include "baygui/Label.h"
#include "baygui/ListBox.h"
#include "baygui/Scrollbar.h"
#include "baygui/TextField.h"
#include "baygui/Container.h"
#include "baygui/Window.h"

#endif // _BAYGUI_H_INCLUDED_
