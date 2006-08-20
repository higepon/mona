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
#include "trash/tpopupmenu.hpp"
//booo
#include "rootwindow.hpp"

using namespace gnote;

namespace trash {
	//
	TPopupMenu::TPopupMenu() : base(0), listener(0) {
	}
	TPopupMenu::~TPopupMenu() {
		for (int ii = 0; ii < items.size(); ii++) {
			delete items.remove(ii);
		}
	}
	//
	void TPopupMenu::processEvent(Event* event) {
		switch (event->getType()) {
			case MouseEvent::MOUSE_MOVED:
				{
					MouseEvent* me = (MouseEvent*)event;
					TMenuItem* m = (TMenuItem*)getComponentAt(getX() + me->getX(), getY() + me->getY());
					if (m) {
						for (int ii = 0; ii < items.size(); ii++) {
							TMenuItem* c = (TMenuItem*)items.get(ii);
							if (c != m && c->getEnabled() && c->getFocused() && c->getType() == TMenuItem::TYPE_ITEM) {
								c->setFocused(false);
								repaint();

							}
						}
						if (m->getEnabled() && !m->getFocused() && m->getType() == TMenuItem::TYPE_ITEM) {
							m->setFocused(true);
							repaint();
						}
					} else {
						for (int ii = 0; ii < items.size(); ii++) {
							TMenuItem* c = (TMenuItem*)items.get(ii);
							if (c->getEnabled() && c->getFocused() && c->getType() == TMenuItem::TYPE_ITEM) {
								c->setFocused(false);
								repaint();
							}
						}
					}
				}
				break;
			case MouseEvent::MOUSE_PRESSED:
				{
					MouseEvent* me = (MouseEvent*)event;
					TMenuItem* m = (TMenuItem*)getComponentAt(getX() + me->getX(), getY() + me->getY());
					if (m && m->getEnabled()) {
						TActionEvent* ae = new TActionEvent();
						ae->setSource(m);
						ae->setActionCommand(m->getActionCommand());
						m->processEvent(ae);
						delete ae;
						hide();
					}
				}
				break;
			default:
				break;
		}
	}
	//
	void TPopupMenu::add(TMenuItem* item) {
		items.add(item);
		Container::add(item);
	}
	//
	void TPopupMenu::show(Component* origin, int x, int y) {
		if (items.size() <= 0) {
			return;
		}
		base = origin;
		int fw = 100;
		FontMetrics* fm = getFontMetrics();
		int fh = fm->getHeight("0") + 6;
		int h = 0;
		for (int ii = 0; ii < items.size(); ii++) {
			TMenuItem* item = (TMenuItem*)items.get(ii);
			switch (item->getType()) {
				case TMenuItem::TYPE_ITEM:
					if (fm->getWidth(item->getTitle()) + 20 > fw) {
						fw = fm->getWidth(item->getTitle()) + 20;
					}
					break;
				default:
					break;
			}

		}
		for (int ii = 0; ii < items.size(); ii++) {
			TMenuItem* item = (TMenuItem*)items.get(ii);
			switch (item->getType()) {
				case TMenuItem::TYPE_ITEM:
					item->setBounds(base->getX() + x + 2, base->getY() + y + h + 2, fw, fh);
					h += fh;
					break;
				case TMenuItem::TYPE_SEPARATOR:
					item->setBounds(base->getX() + x + 2, base->getY() + y + h + 2, fw, 8);
					h += 8;
					break;
			}

		}
		setBounds(base->getX() + x, base->getY() + y, fw + 4, h + 4);
		//boo
		RootWindow* root = (RootWindow*)base->getMainWindow();
		if (root) {
			root->ShowPopupMenu(this);
		}
	}
	//
	void TPopupMenu::hide() {
		//boo
		RootWindow* root = (RootWindow*)base->getMainWindow();
		if (root) {
			root->HidePopupMenu();
		}
	}
	//
	void TPopupMenu::paint(Graphics* g) {
		g->setColor(Color::white);
		g->drawLine(0, 0, getWidth() - 1, 0);
		g->drawLine(0, 0, 0, getHeight() - 1);
		g->setColor(getForeground());
		g->drawLine(0, getHeight() - 1, getWidth() - 1, getHeight() - 1);
		g->drawLine(getWidth() - 1, 0, getWidth() - 1, getHeight() - 1);
	}

	//
	void TPopupMenu::repaint() {
		Graphics* g = getGraphics();
		if (!g) {
			return;
		}
		paint(g);
		for (int ii = 0; ii < items.size(); ii++) {
			TMenuItem* item = (TMenuItem *)items.get(ii);
			item->paintAt(g, item->getX() - getX(), item->getY() - getY());
		}
		update();
	}
	//
	void TPopupMenu::setEventListener(TEventListener* listener) {
		this->listener = listener;
		for (int ii = 0; ii < items.size() ;ii++) {
			TMenuItem* item = (TMenuItem*)items.get(ii);
			item->setEventListener(listener);
		}
	}
}
