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

#include <baygui.h>
#include "htmlpanel.h"

class GBrowser : public Frame {

private:
	TextField *address;
	Button *reload, *stop, *home, *favorites, *go;
	HtmlPanel *panel;

public:
	GBrowser(){
		setBounds((800 - 212) / 2, (600 - 228) /2, 212, 228);
		setTitle("ブラウザもどき");
		// いろいろな部品の作成
		reload = new Button("更新");
		stop = new Button("中止");
		home = new Button("ホーム");
		favorites = new Button("お気に入り");
		go = new Button("移動");
		address = new TextField();
		panel = new HtmlPanel();
		// いろいろな部品の大きさを設定
		reload->setBounds(0,0,40,20);
		stop->setBounds(40,0,40,20);
		home->setBounds(80,0,50,20);
		favorites->setBounds(130,0,70,20);
		go->setBounds(160,20,40,20);
		address->setBounds(0,20,160,20);
		panel->setBounds(0,40,200,160);
		// いろいろな部品をウィンドウに貼り付ける
		add(address);
		add(reload);
		add(stop);
		add(home);
		add(favorites);
		add(go);
		add(panel);
	}
	
	~GBrowser(){
		free(address);
		free(reload);
		free(stop);
		free(home);
		free(favorites);
		free(go);
		free(panel);
	}
	
	void processEvent(Event *event){
		if (event->getType() == MouseEvent::MOUSE_RELEASED) {
			// 更新を押したとき
			if (event->getSource() == reload) {
				// HtmlPanelクラスに処理をお願いする
				panel->setUrl(address->getText());
			// 中止を押したとき
			} else if (event->getSource() == stop) {
				address->setText("");
				// HtmlPanelクラスに処理をお願いする
				panel->setUrl("about:stop");
			// ホームを押したとき
			} else if (event->getSource() == home) {
				address->setText("about:home");
				// HtmlPanelクラスに処理をお願いする
				panel->setUrl("about:home");
			// お気に入りを押したとき
			} else if (event->getSource() == favorites) {
				address->setText("about:favorites");
				// HtmlPanelクラスに処理をお願いする
				panel->setUrl("about:favorites");
			// 移動を押したとき
			} else if (event->getSource() == go) {
				// HtmlPanelクラスに処理をお願いする
				panel->setUrl(address->getText());
			}
		} else if (event->getType() == Event::TEXT_CHANGED) {
			// 確定したとき
			if (event->getSource() == address) {
				// HtmlPanelクラスに処理をお願いする
				panel->setUrl(address->getText());
			}
		}
	}
};

int MonaMain(List<char*>* pekoe) {
	GBrowser *browser = new GBrowser();
	browser->run();
	delete(browser);
	return 0;
}
