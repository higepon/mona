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

// ==================================================
// OS 定義部分
// ==================================================
//#define PEKOE 1
#define MONA  1
//#define OSASK 1

// ==================================================
// OS 依存ヘッダファイル
// ==================================================
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

// ==================================================
// BayGUI グローバルマクロ
// ==================================================
#define inGetUInt16(b) (unsigned short)( (unsigned short)((b)[1])<<8 | (unsigned short)((b)[0]) )
#define inGetUInt32(b) (unsigned int)( (unsigned int)((b)[3])<<24 | (unsigned int)((b)[2])<<16 | (unsigned int)((b)[1])<<8 | (unsigned int)((b)[0]) )
#define inGetUInt16(b) (unsigned short)( (unsigned short)((b)[1])<<8 | (unsigned short)((b)[0]) )

// ==================================================
// BayGUI ヘッダファイル
// ==================================================
#include "baygui/messages.h"
#include "baygui/resources.h"
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
