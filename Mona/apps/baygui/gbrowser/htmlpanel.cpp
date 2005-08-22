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

/** コンストラクタ */
HtmlPanel::HtmlPanel(){
}

/** URLを指定する */
void HtmlPanel::setUrl(char *url){
	strcpy(this->url, url);
	// ここで再描画をしている
	repaint();
}

/** 再描画（repaintを呼ぶとここにくる）*/
void HtmlPanel::paint(Graphics *g){
	// 背景を灰色で塗りつぶす
	g->setColor(getBackground());
	g->fillRect(0, 0, getWidth(), getHeight());
	
	// 遷移先を振り分ける
	if (strcmp(this->url, "about:home") == 0) {
		// ホームを押したときはここへ
		g->setColor(0,0,0);
		g->drawString("ホームページ", 60, 0);
		g->drawString("ようこそ！", 0, 16);
	} else if (strcmp(this->url, "about:stop") == 0) {
		// 中止を押したときはここへ
		g->setColor(255,0,0);
		g->drawString("取り消されたアクション", 0, 0);
	} else if (strcmp(this->url, "about:favorites") == 0) {
		// お気に入りを押したときはここへ
		g->setColor(0,0,0);
		g->drawString("お気に入り", 0, 0);
		g->setColor(0,0,255);
		g->drawString("・仕事関係", 0, 16);
		g->drawString("・プライベート", 0, 32);
	} else if (url != NULL && strlen(url) > 0) {
		// URLをなにか入れて移動を押したときはここへ
		g->setColor(0,0,0);
		g->drawString("HTTP404エラー", 0, 0);
	}
}
