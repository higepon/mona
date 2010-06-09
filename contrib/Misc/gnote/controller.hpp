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
#ifndef __GNOTE_CONTROLLER_HPP__
#define __GNOTE_CONTROLLER_HPP__

#include "baygui.h"
#include "baygui/awt/event/TimerEvent.h"
#include "trash/tactionevent.hpp"
#include "trash/teventlistener.hpp"
#include "rootwindow.hpp"
#include "document.hpp"
#include "cursol.hpp"
#include "canvas.hpp"

using namespace trash;

namespace gnote {
	class Controller : public TEventListener {
	public:
		enum {
			TIMER_INTERVAL = 10,
			BLINK_TIMER_INTERVAL = 500
		};
	protected:
		RootWindow window;
		Document document;
		Cursol cursol;
		Document clip;
		int keyCode;
		int keyModifiers;
		int counter;
		String file;
		KeyEvent prevKeyEvent;
	public:
		Controller();
		virtual ~Controller() {}
		inline void SetFile(const String& file) { this->file = file; }
		void Run();
		void Exit();
		String ToCanvasString(const String& s);
		String ToCanvasString(const Document& d, int wy);
		virtual void ProcessEvent(Event* event);
		virtual void ProcessActionEvent(TActionEvent* event);
		virtual void ProcessTimerEvent(TimerEvent* event);
		virtual void ProcessKeyPressedEvent(KeyEvent* event);
		virtual void processEvent(Event* event) { ProcessEvent(event); }
	protected:
		bool ReadFile(const String& f, Document& d);
		bool WriteFile(const String& f, const Document& d);
		int ToCanvasY(int wy, int t = 1);
		int ToCanvasX(int wy, int wx, int l = 1);
		int ToWorldX(int wy, int cx, int l = 1);
		void SetCursolRange(Cursol& c, int modifiers);
		bool Go(Cursol& c, const Document& d, int y, int x);
		bool GoRight(Cursol& c, const Document& d, int l = 1);
		bool GoLeft(Cursol& c, const Document& d, int l = 1);
		bool GoUp(Cursol& c, const Document& d, int l = 1);
		bool GoDown(Cursol& c, const Document& d, int l = 1);
		bool GoHome(Cursol& c, const Document& d);
		bool GoEnd(Cursol& c, const Document& d);
		bool GoHead(Cursol& c, const Document& d);
		bool GoTail(Cursol& c, const Document& d);
		bool PageDown(Cursol& c, const Document& d);
		bool PageUp(Cursol& c, const Document& d);
		bool Delete(Cursol& c, Document& d);
		bool BackSpace(Cursol& c, Document& d);
		bool Copy(Cursol& c, const Document& d, Document& cl);
		bool Cut(Cursol& c, Document& d, Document& cl);
		bool Paste(Cursol& c, Document& d, const Document& cl);
		bool New(Cursol& c, Document& d);
		bool isPrefixCtrlX();
	};

}

#endif
