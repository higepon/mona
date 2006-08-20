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
#include "trash/tmenubar.hpp"

namespace trash {
	//
	TMenuBar::~TMenuBar() {
		for (int ii = 0; ii < menus.size(); ii++) {
			delete menus.remove(ii);
		}
	}
	//
	void TMenuBar::setEventListener(TEventListener* l) {
		listener = l;
		for (int ii = 0; ii < menus.size() ; ii++) {
			TMenu* m = (TMenu*)menus.get(ii);
			m->setEventListener(l);
		}
	}
	//
	void TMenuBar::add(TMenu* m) {
		FontMetrics* fm = getFontMetrics();
		int x = 2;
		for (int ii = 0; ii < menus.size(); ii++) {
			TMenu* c = (TMenu*)menus.get(ii);
			x += fm->getWidth(c->getTitle()) + 20;
		}
		m->setBounds(x + getX(), getY() + 1, fm->getWidth(m->getTitle()) + 20, getHeight() - 3);
		menus.add(m);
		Container::add(m);
	}
	//
	void TMenuBar::processEvent(Event* event) {
		switch (event->getType()) {
			case MouseEvent::MOUSE_MOVED:
				{
					MouseEvent* me = (MouseEvent*)event;
					TMenu* m = (TMenu*)getComponentAt(getX() + me->getX(), getY() + me->getY());
					if (m) {
						for (int ii = 0; ii < menus.size(); ii++) {
							TMenu* c = (TMenu*)menus.get(ii);
							if (c != m && c->getFocused()) {
								c->setFocused(false);
								c->repaint();
							}
						}
						if (!m->getFocused()) {
							m->setFocused(true);
							m->repaint();
							if (isSelected) {
								this->event.setSource(m);
								m->processEvent(&this->event);
							}
						}
					} else {
						for (int ii = 0; ii < menus.size(); ii++) {
							TMenu* c = (TMenu*)menus.get(ii);
							if (c->getFocused()) {
								c->setFocused(false);
								c->repaint();
							}
						}
					}
				}
				break;
			case MouseEvent::MOUSE_PRESSED:
				{
					MouseEvent* me = (MouseEvent*)event;
					TMenu* m = (TMenu*)getComponentAt(getX() + me->getX(), getY() + me->getY());
					if (m) {
						this->event.setSource(m);
						m->processEvent(&this->event);
						isSelected = true;
					}
				}
				break;
			default:
				break;
		}
	}
	//
	void TMenuBar::paint(Graphics* g) {
		g->setColor(getBackground());
		g->fillRect(0, 0, getWidth(), getHeight());
		g->setColor(Color::white);
		g->drawLine(0, 0, getWidth() - 1, 0);
		g->drawLine(0, 0, 0, getHeight() - 1);
		g->setColor(getForeground());
		g->drawLine(0, getHeight() - 1, getWidth() - 1, getHeight() - 1);
	}
	//
	void TMenuBar::repaint() {
		Graphics* g = getGraphics();
		if (!g) {
			return;
		}
		paint(g);
		for (int ii = 0; ii < menus.size(); ii++) {
			TMenu* m = (TMenu*)menus.get(ii);
			m->paintAt(g, m->getX() - getX(), m->getY() - getY());
		}
		update();
	}
}
