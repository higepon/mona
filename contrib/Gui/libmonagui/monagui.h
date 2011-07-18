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

#if !defined(_MONAGUI_H_INCLUDED_)
#define _MONAGUI_H_INCLUDED_

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
#include "monagui_SDL.h"
#endif

// ==================================================
// Monagui グローバルマクロ
// ==================================================

#define MONAGUI_VERSION "Monagui $Date$"
#define MONAGUI_COPYING "Copyright (C) 2004-2005, bayside."

// ==================================================
// Monagui ヘッダファイル
// ==================================================

#include "monagui/awt/Font.h"
#include "monagui/awt/Color.h"
#include "monagui/lang/Object.h"
#include "monagui/lang/Pointer.h"
#include "monagui/lang/String.h"
#include "monagui/lang/Runnable.h"
#include "monagui/lang/Thread.h"
#include "monagui/util/Vector.h"
#include "monagui/util/Hashtable.h"
#include "monagui/awt/Event.h"
#include "monagui/awt/event/ActionEvent.h"
#include "monagui/awt/event/ActionListener.h"
#include "monagui/awt/event/AdjustmentEvent.h"
#include "monagui/awt/event/AdjustmentListener.h"
#include "monagui/awt/event/FocusEvent.h"
#include "monagui/awt/event/FocusListener.h"
#include "monagui/awt/event/ItemEvent.h"
#include "monagui/awt/event/ItemListener.h"
#include "monagui/awt/event/KeyEvent.h"
#include "monagui/awt/event/KeyListener.h"
#include "monagui/awt/event/MouseEvent.h"
#include "monagui/awt/event/MouseEvent.h"
#include "monagui/awt/event/TextEvent.h"
#include "monagui/awt/event/TextListener.h"
#include "monagui/awt/event/TimerEvent.h"
#include "monagui/awt/event/TimerListener.h"
#include "monagui/awt/event/WindowEvent.h"
#include "monagui/awt/event/WindowListener.h"
#include "monagui/awt/Insets.h"
#include "monagui/awt/Rectangle.h"
#include "monagui/awt/FontMetrics.h"
#include "monagui/awt/Image.h"
#include "monagui/awt/WebImage.h"
#include "monagui/awt/Graphics.h"
#include "monagui/awt/Component.h"
#include "monagui/awt/Checkbox.h"
#include "monagui/awt/CheckboxGroup.h"
#include "monagui/awt/Button.h"
#include "monagui/awt/Label.h"
#include "monagui/awt/ListBox.h"
#include "monagui/awt/Scrollbar.h"
#include "monagui/awt/Container.h"
#include "monagui/awt/TextField.h"
#include "monagui/awt/Window.h"
#include "monagui/awt/Frame.h"
#include "monagui/util/MonaGUIRobot.h"
#include "monagui/awt/ImageIcon.h"
#include "monagui/awt/Canvas.h"

using namespace monagui;

#endif // _MONAGUI_H_INCLUDED_
