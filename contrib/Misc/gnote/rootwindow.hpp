/*
 Copyright (c) 2005 hetappi
 All rights reserved.

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to
 deal in the Software without restriction, including without limitation the
 rights to use, copy, modify, merge, publish, distribute, and/or sell copies
 of the Software, and to permit persons to whom the Software is furnished to
 do so, provided that the above copyright notice(s) and this permission
 notice appear in all copies of the Software and that both the above
 copyright notice(s) and this permission notice appear in supporting
 documentation.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF THIRD PARTY RIGHTS.
 IN NO EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS INCLUDED IN THIS NOTICE BE
 LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT OR CONSEQUENTIAL DAMAGES, OR
 ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
 IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

 Except as contained in this notice, the name of a copyright holder shall not
 be used in advertising or otherwise to promote the sale, use or other
 dealings in this Software without prior written authorization of the
 copyright holder.
*/
#ifndef __GNOTE_ROOTWINDOW_HPP__
#define __GNOTE_ROOTWINDOW_HPP__

#include "baygui.h"

#include "trash/tmenubar.hpp"
#include "trash/tpopupmenu.hpp"
#include "trash/teventlistener.hpp"

#include "canvas.hpp"

using namespace trash;

namespace gnote {
	class RootWindow : public Frame {
	protected:
		TEventListener* listener;
		Canvas* canvas;
		TMenuBar* menuBar;
		TPopupMenu* popupMenu;
	public:
		RootWindow();
		virtual ~RootWindow();
		void SetEventListener(TEventListener* l);
		inline void SetDocument(Document* d) { canvas->SetDocument(d); }
		inline Canvas* GetCanvas() { return canvas; }
		void ShowPopupMenu(TPopupMenu* menu);
		void HidePopupMenu();
		inline bool IsPopuped() { return popupMenu != 0; }
		void processEvent(Event* event);
	};
}

#endif
