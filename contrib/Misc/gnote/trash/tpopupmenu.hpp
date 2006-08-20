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
#ifndef __TRASH_TPOPUPMENU_HPP__
#define __TRASH_TPOPUPMENU_HPP__

#include "baygui.h"
#include "trash/tmenuitem.hpp"
#include "trash/teventlistener.hpp"

namespace trash {
	class TPopupMenu : public Container {
	protected:
		Vector items;
		Component* base;
		TEventListener* listener;
	private:
		virtual void add(Component* c) {}
	public:
		TPopupMenu();
		virtual ~TPopupMenu();
		void setEventListener(TEventListener* listener);
		void show(Component* base, int x, int y);
		void hide();
		virtual void paint(Graphics* g);
		virtual void repaint();
		virtual void processEvent(Event* event);
		virtual void add(TMenuItem* item);
	};
}

#endif
