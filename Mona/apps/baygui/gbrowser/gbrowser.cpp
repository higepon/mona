/*
Copyright (c) 2004 bayside
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. The name of the author may not be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <baygui.h>
#include "htmlpanel.h"

class GBrowser : public Window {

private:
	TextField *address;
	Button *reload, *stop, *home, *favorites, *go;
	HtmlPanel *panel;

public:
	GBrowser(){
		setRect((800 - 213) / 2, (600 - 229) /2, 213, 229);
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
		reload->setRect(0,0,40,20);
		stop->setRect(40,0,40,20);
		home->setRect(80,0,50,20);
		favorites->setRect(130,0,70,20);
		go->setRect(160,21,40,20);
		address->setRect(0,21,159,20);
		panel->setRect(1,44,200,140);
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
	
	void onEvent(Event *event){
		// 更新を押したとき
		if (event->source == reload) {
			// HtmlPanelクラスに処理をお願いする
			panel->setUrl(address->getText());
		// 中止を押したとき
		} else if (event->source == stop) {
			address->setText("");
			// HtmlPanelクラスに処理をお願いする
			panel->setUrl("about:stop");
		// ホームを押したとき
		} else if (event->source == home) {
			address->setText("about:home");
			// HtmlPanelクラスに処理をお願いする
			panel->setUrl("about:home");
		// お気に入りを押したとき
		} else if (event->source == favorites) {
			address->setText("about:favorites");
			// HtmlPanelクラスに処理をお願いする
			panel->setUrl("about:favorites");
		// 移動を押したとき
		} else if (event->source == go || event->type == TEXT_CHANGED) {
			// HtmlPanelクラスに処理をお願いする
			panel->setUrl(address->getText());
		}
	}
};

int MonaMain(List<char*>* pekoe) {
	GBrowser *browser = new GBrowser();
	browser->run();
	delete(browser);
	return 0;
}
