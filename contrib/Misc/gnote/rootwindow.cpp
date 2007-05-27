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
#include "rootwindow.hpp"

namespace gnote {
	//
	RootWindow::RootWindow() : listener(0), canvas(0), menuBar(0), popupMenu(0) {
		int cw;
		int ch;
		//
		canvas = new Canvas();
		canvas->setBounds(
			0, 20,
			cw = canvas->GetGridWidth() * (Canvas::MAX_CANVASX + 5),
			ch = canvas->GetGridHeight() * (Canvas::MAX_CANVASY + 1));
		add(canvas);

		//
		menuBar = new TMenuBar();
		menuBar->setBounds(0, 0, cw, 20);
		add(menuBar);

		//ファイルメニュー
		TMenuItem* item;
		TMenu* menu = new TMenu("File");
		menu->add(item = new TMenuItem("New"));
		item->setActionCommand("FILE_NEW");
		menu->add(item = new TMenuItem("Open..."));
		item->setEnabled(false);
		menu->add(item = new TMenuItem("Save"));
		item->setEnabled(false);
		menu->add(item = new TMenuItem("Save As..."));
		item->setEnabled(false);
		menu->add(item = new TMenuItem(TMenuItem::TYPE_SEPARATOR));
		menu->add(item = new TMenuItem("Quit"));
		item->setActionCommand("FILE_QUIT");
		menuBar->add(menu);

		//編集メニュー
		menuBar->add(menu = new TMenu("Edit"));
		menu->add(item = new TMenuItem("Undo"));
		item->setEnabled(false);
		menu->add(item = new TMenuItem(TMenuItem::TYPE_SEPARATOR));
		menu->add(item = new TMenuItem("Cut"));
		item->setActionCommand("EDIT_CUT");
		menu->add(item = new TMenuItem("Copy"));
		item->setActionCommand("EDIT_COPY");
		menu->add(item = new TMenuItem("Paste"));
		item->setActionCommand("EDIT_PASTE");
		menu->add(item = new TMenuItem("Delete"));
		item->setActionCommand("EDIT_DELETE");
		menu->add(item = new TMenuItem(TMenuItem::TYPE_SEPARATOR));
		menu->add(item = new TMenuItem("Select All"));
		item->setActionCommand("EDIT_SELECTALL");
		menu->add(item = new TMenuItem(TMenuItem::TYPE_SEPARATOR));
		menu->add(item = new TMenuItem("Find..."));
		item->setEnabled(false);
		menu->add(item = new TMenuItem("Replace..."));
		item->setEnabled(false);
		menu->add(item = new TMenuItem(TMenuItem::TYPE_SEPARATOR));
		menu->add(item = new TMenuItem("Jump..."));
		item->setEnabled(false);

		//
		menuBar->add(menu = new TMenu("Help"));
		menu->add(item = new TMenuItem("Version..."));
		item->setEnabled(false);

		//
		setTitle("note");
	}
	//
	RootWindow::~RootWindow() {
		if (canvas) {
			delete canvas;
		}
		if (menuBar) {
			delete menuBar;
		}
	}
	//
	void RootWindow::SetEventListener(TEventListener* l) {
		listener = l;
		if (canvas) {
			canvas->SetEventListener(l);
		}
		if (menuBar) {
			menuBar->setEventListener(l);
		}
	}
	//
	void RootWindow::processEvent(Event* event) {
		if (IsPopuped()) {
			switch (event->getType()) {
				case MouseEvent::MOUSE_PRESSED:
					if (popupMenu != getComponentAt(getX(), getY())) {
						HidePopupMenu();
						if (menuBar) {
							menuBar->setSelected(false);
						}
					}
					break;
				default:
					break;
			}
		}
		if (listener) {
			listener->processEvent(event);
		}
	}
	//
	void RootWindow::HidePopupMenu() {
		if (popupMenu) {
			popupMenu->setFocused(false);
			remove(popupMenu);
			popupMenu = 0;
		}
		canvas->repaint();
	}
	void RootWindow::ShowPopupMenu(TPopupMenu* menu) {
		if (popupMenu) {
			popupMenu->setFocused(false);
			remove(popupMenu);
			popupMenu = 0;
		}
		popupMenu = menu;
		add(menu);
		menu->setFocused(true);
		canvas->repaint();
		popupMenu->repaint();
	}
}
