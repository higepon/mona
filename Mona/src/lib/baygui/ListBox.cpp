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

#include "baygui.h"

namespace baygui {
	ListBox::ListBox()
	{
		this->selectedIndex =  -1;
		this->itemEvent.setType(Event::ITEM_SELECTED);
		this->itemEvent.setSource(this);
		setBackground(Color::white);
	}

	ListBox::~ListBox()
	{
		int I = this->dataList.size();
		for (int i = 0; i < I; i++) {
			String* s = (String *)this->dataList.remove(0);
			delete(s);
		}
	}

	char* ListBox::getSelectedItem()
	{
		String* s = (String *)dataList.get(this->selectedIndex);
		return (s == NULL) ? NULL : s->getBytes();
	}

	void ListBox::select(int index)
	{
		if (0 <= index && index < this->dataList.size()) {
			this->selectedIndex = index;
			repaint();
		}
	}

	void ListBox::add(char* item)
	{
		this->dataList.add(new String(item));
	}

	void ListBox::remove(int index)
	{
		String* s = (String *)this->dataList.remove(index);
		delete(s);
	}

	void ListBox::paint(Graphics* g)
	{
		int w = getWidth();
		int h = getHeight();
		
		// 外枠
		if (getFocused() == true && getEnabled() == true) {
			g->setColor(0,128,255);
			g->drawRect(0, 0, w, h);
		} else {
			g->setColor(getParent()->getBackground());
			g->drawRect(0, 0, w, h);
		}
		
		// 内枠
		g->setColor(getBackground());
		g->fillRect(1, 1, w - 2, h - 2);
		g->setColor(getForeground());
		g->drawRect(1, 1, w - 2, h - 2);

		// 文字
		int I = this->dataList.size();
		for (int i = 0; i < I; i++) {
			if (selectedIndex == i && getEnabled() == true) {
				g->setColor(0, 128, 255);
				g->fillRect(3, (16 * i) + 3, w - 6, 16);
				g->setColor(getBackground());
				g->drawString(((String *)dataList.get(i))->getBytes(), 4, (16 * i) + 6);
			} else {
				g->setColor(getForeground());
				g->drawString(((String *)dataList.get(i))->getBytes(), 4, (16 * i) + 6);
			}
		} 
	}

	void ListBox::processEvent(Event* event)
	{
		// 非活性の時はイベントを受け付けない
		if (getEnabled() == false) return;

		// キー押下
		if (event->getType() == KeyEvent::KEY_PRESSED) {
			int keycode = ((KeyEvent *)event)->getKeycode();
			if (keycode == KeyEvent::VKEY_UP) {
				if (this->selectedIndex > 0) {
					this->selectedIndex--;
					repaint();
					getParent()->processEvent(&this->itemEvent);
				}
			} else if (keycode == KeyEvent::VKEY_DOWN) {
				if (this->selectedIndex < this->dataList.size() - 1 && 
					this->selectedIndex < ((getHeight() - 22) / 16))
				{
					this->selectedIndex++;
					repaint();
					getParent()->processEvent(&this->itemEvent);
				}
			} else if (keycode == KeyEvent::VKEY_ENTER) {
				getParent()->processEvent(&this->itemEvent);
			}
		// マウス押下
		} else if (event->getType() == MouseEvent::MOUSE_PRESSED) {
			int my = ((MouseEvent *)event)->getY();
			//printf("y = %d,", my);
			select((my - 3) / 16);
			getParent()->processEvent(&this->itemEvent);
		// フォーカス状態変更
		} else if (event->getType() == Event::FOCUS_IN || event->getType() == Event::FOCUS_OUT) {
			repaint();
			getParent()->processEvent(&this->itemEvent);
		}
	}
}
