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
void HtmlPanel::onPaint(Graphics *g){
	// 背景を灰色で塗りつぶす
	g->setColor(200, 200, 200);
	g->fillRect(0, 0, width, height);
	
	// 遷移先を振り分ける
	if (strcmp(this->url, "about:home") == 0) {
		// ホームを押したときはここへ
		g->setColor(0,0,0);
		g->drawText("ホームページ", 60, 0);
		g->drawText("ようこそ！", 0, 16);
	} else if (strcmp(this->url, "about:stop") == 0) {
		// 中止を押したときはここへ
		g->setColor(255,0,0);
		g->drawText("取り消されたアクション", 0, 0);
	} else if (strcmp(this->url, "about:favorites") == 0) {
		// お気に入りを押したときはここへ
		g->setColor(0,0,0);
		g->drawText("お気に入り", 0, 0);
		g->setColor(0,0,255);
		g->drawText("・仕事関係", 0, 16);
		g->drawText("・プライベート", 0, 32);
	} else if (url != NULL && strlen(url) > 0) {
		// URLをなにか入れて移動を押したときはここへ
		g->setColor(0,0,0);
		g->drawText("HTTP404エラー", 0, 0);
	}
}
