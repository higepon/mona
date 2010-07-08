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
#include <servers/gui.h>
#elif defined(SDL)
#include "baygui_SDL.h"
#endif

// ==================================================
// BayGUI グローバルマクロ
// ==================================================

#define BAYGUI_VERSION "BayGUI $Date$"
#define BAYGUI_COPYING "Copyright (C) 2004-2005, bayside."

// ==================================================
// BayGUI ヘッダファイル
// ==================================================

#include "baygui/awt/Font.h"
#include "baygui/awt/Color.h"
#include "baygui/lang/Object.h"
#include "baygui/lang/Pointer.h"
#include "baygui/lang/String.h"
#include "baygui/lang/Runnable.h"
#include "baygui/lang/Thread.h"
#include "baygui/util/Vector.h"
#include "baygui/util/Hashtable.h"
#include "baygui/awt/Event.h"
#include "baygui/awt/event/ActionEvent.h"
#include "baygui/awt/event/ActionListener.h"
#include "baygui/awt/event/AdjustmentEvent.h"
#include "baygui/awt/event/AdjustmentListener.h"
#include "baygui/awt/event/FocusEvent.h"
#include "baygui/awt/event/FocusListener.h"
#include "baygui/awt/event/ItemEvent.h"
#include "baygui/awt/event/ItemListener.h"
#include "baygui/awt/event/KeyEvent.h"
#include "baygui/awt/event/KeyListener.h"
#include "baygui/awt/event/MouseEvent.h"
#include "baygui/awt/event/MouseEvent.h"
#include "baygui/awt/event/TextEvent.h"
#include "baygui/awt/event/TextListener.h"
#include "baygui/awt/event/TimerEvent.h"
#include "baygui/awt/event/TimerListener.h"
#include "baygui/awt/event/WindowEvent.h"
#include "baygui/awt/event/WindowListener.h"
#include "baygui/awt/Insets.h"
#include "baygui/awt/Rectangle.h"
#include "baygui/awt/FontMetrics.h"
#include "baygui/awt/Image.h"
#include "baygui/awt/Graphics.h"
#include "baygui/awt/Component.h"
#include "baygui/awt/Checkbox.h"
#include "baygui/awt/CheckboxGroup.h"
#include "baygui/awt/Button.h"
#include "baygui/awt/Label.h"
#include "baygui/awt/ListBox.h"
#include "baygui/awt/Scrollbar.h"
#include "baygui/awt/Container.h"
#include "baygui/awt/TextField.h"
#include "baygui/awt/Window.h"
#include "baygui/awt/Frame.h"

using namespace baygui;

#endif // _BAYGUI_H_INCLUDED_
