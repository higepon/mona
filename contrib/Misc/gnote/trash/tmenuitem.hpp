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
#ifndef __TRASH_TMENUITEM_HPP__
#define __TRASH_TMENUITEM_HPP__

#define COLOR_BLUEGRAY 0xffdfdfff

#include "baygui.h"
#include "trash/teventlistener.hpp"

namespace trash {
	class TMenuItem : public Component {
	public:
		enum Type {
			TYPE_ITEM,
			TYPE_SEPARATOR
		};
	protected:
		String title;
		String command;
		Type type;
		TEventListener* listener;
	public:
		TMenuItem(Type type, const char* title = 0);
		TMenuItem(const char* title);
		virtual ~TMenuItem() {}
		inline String getTitle() const { return title; }
		inline Type getType() const { return type; }
		inline void setActionCommand(const char* command) { this->command = command; }
		inline String getActionCommand() const { return command; }
		inline void setEventListener(TEventListener* listener) { this->listener = listener; }
		virtual void processEvent(Event* event);
		virtual void paint(Graphics* g);
		virtual void paintAt(Graphics* g, int x, int y);
	};
}

#endif
