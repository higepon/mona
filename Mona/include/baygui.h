/*
Copyright (c) 2004 bayside
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. The name of the author may not be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#if !defined(_BAYGUI_H_INCLUDED_)
#define _BAYGUI_H_INCLUDED_

//#define PEKOE 1
#define MONA  1
//#define OSASK 1

#if defined(PEKOE)
	#include "baygui.h"
	#include "stdlib.h"
	#include "stdint.h"
	#include "stdio.h"
	#include "string.h"
	#include "fcntl.h"
	#include "rpcss.h"
	#include "sys/gbc.h"
#elif defined(MONA)
	#include <monapi.h>
	#include <monapi/messages.h>
	#include <monalibc.h>
#endif

#define GUILIB_VERSION "BayGUI build 04.10.24"
#define GUISERVER_NAME "BAYGUI.EX5"

#define MAX_TITLE_LEN     64
#define MAX_TEXT_LEN      64
#define MAX_FONTLIST_LEN  96

#define INSETS_TOP        22
#define INSETS_BOTTOM     6
#define INSETS_LEFT       6
#define INSETS_RIGHT      6

#define STATE_NORMAL      1
#define STATE_MOVING      2

#define UNKNOWN_IMAGE     0
#define BITMAP_IMAGE      1
#define ICON_IMAGE        2

#define ALIGN_LEFT        1
#define ALIGN_CENTER      2
#define ALIGN_RIGHT       3

/** キー押下 */
#define KEY_PRESSED     100
/** キーリリース */
#define KEY_RELEASED    101
/** マウスクリック */
#define MOUSE_PRESSED   200
/** マウスリリース */
#define MOUSE_RELEASED  201
/** マウスドラッグ */
#define MOUSE_DRAGGED   202
/** マウス移動 */
#define MOUSE_MOVED     203
/** フォーカスイン */
#define FOCUS_IN        300
/** フォーカスアウト */
#define FOCUS_OUT       301
/** タイマー */
#define TIMER           302
/** テキスト確定 */
#define TEXT_CHANGED    303
/** 項目選択 */
#define ITEM_SELECTED   304
/** ブロック増加 */
#define BLOCK_INCLEMENT 305
/** ブロック減少 */
#define BLOCK_DECLEMENT 306

//
// ↓特殊キーコード一覧
//

#define VKEY_ESC          0
#define VKEY_BACKSPACE   14
#define VKEY_TAB         15
#define VKEY_ENTER       28

#define VKEY_PGUP   354
#define VKEY_PGDOWN 355
#define VKEY_INSERT 356
#define VKEY_DELETE 357
#define VKEY_UP     358
#define VKEY_LEFT   359
#define VKEY_RIGHT  360
#define VKEY_DOWN   361
#define VKEY_HOME   362
#define VKEY_END    363

#define VKEY_F11    382
#define VKEY_F12    383
#define VKEY_F1     398
#define VKEY_F2     399
#define VKEY_F3     400
#define VKEY_F4     401
#define VKEY_F5     402
#define VKEY_F6     403
#define VKEY_F7     404
#define VKEY_F8     405
#define VKEY_F9     406
#define VKEY_F10    407
#define VKEY_LSHIFT 412
#define VKEY_ALT    415
#define VKEY_CTRL   416
#define VKEY_RMENU  424
#define VKEY_LMENU  425
#define VKEY_RSHIFT 427

//
// ↑特殊キーコード一覧
//

#define inGetUInt16(b) (unsigned short)( (unsigned short)((b)[1])<<8 | (unsigned short)((b)[0]) )
#define inGetUInt32(b) (unsigned int)( (unsigned int)((b)[3])<<24 | (unsigned int)((b)[2])<<16 | (unsigned int)((b)[1])<<8 | (unsigned int)((b)[0]) )
#define inGetUInt16(b) (unsigned short)( (unsigned short)((b)[1])<<8 | (unsigned short)((b)[0]) )

#include "baygui/messages.h"
#include "baygui/Object.h"
#include "baygui/String.h"
#include "baygui/LinkedItem.h"
#include "baygui/LinkedList.h"
#include "baygui/Event.h"
#include "baygui/KeyEvent.h"
#include "baygui/MouseEvent.h"
#include "baygui/Rect.h"
#include "baygui/Font.h"
#include "baygui/FontManager.h"
#include "baygui/Image.h"
#include "baygui/Bitmap.h"
#include "baygui/Icon.h"
#include "baygui/Graphics.h"
#include "baygui/Control.h"
#include "baygui/Button.h"
#include "baygui/Label.h"
#include "baygui/ListBox.h"
#include "baygui/TextField.h"
#include "baygui/Container.h"
#include "baygui/Window.h"

#endif // _BAYGUI_H_INCLUDED_
