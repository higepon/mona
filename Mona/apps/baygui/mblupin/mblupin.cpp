/*
Copyright (c) 2005 bayside
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

#include "TITLE.h"

class MbLupin : public Window {
private:
	char  titleString[256];
	wchar titleCharcode;
	MonAPI::Date date;
	MonAPI::Random rnd;

private:
	void drawTitle()
	{
		memset(this->titleString, 0, sizeof(this->titleString));
		strcpy(this->titleString, Titles[rnd.nextInt() % MAX_TITLE]);
		String s = this->titleString;
		
		// １文字づつ描画
		for (int i = 0; i < s.length(); i++) {
			this->titleCharcode = s[i];
			repaint();
			sleep(250);
		}
		
		// タイトル全部を表示
		this->titleCharcode = 0xFFFFFFFF;
		repaint();
		setTimer(2000);
	}

public:
	/** コンストラクタ */
	MbLupin()
	{
		setRect((800 - 300) / 2,(600 - 200) / 2,300,200);
		setTitle("mblupin");
		
		// ランダムのシード値を与える。
		date.refresh();
		rnd.setSeed(date.hour() * 3600 + date.min() * 60 + date.sec());
		this->titleCharcode = 0;
	}
	
	/** デストラクタ */
	~MbLupin()
	{
	}
	
	/** 起動ハンドラ */
	void onStart()
	{
		Window::onStart();
		drawTitle();
	}
	
	/** 描画ハンドラ */
	void onPaint(Graphics *g)
	{
		int  x, y, offset, width, height, pos, bit;
		char fp[256];
		
		x = y = pos = 0;
		bit = 1;
		
		if (this->titleCharcode == 0) return;
		
		// バックを黒に塗る
		g->setColor(Color::BLACK);
		g->fillRect(0,0,getWidth(),getHeight());
		
		// タイトル全部を表示
		if (this->titleCharcode == 0xFFFFFFFF) {
			x = (getWidth() - Window::INSETS_LEFT - Window::INSETS_RIGHT 
				- getFontMetrics()->getWidth(this->titleString)) / 2;
			y = (getHeight() - Window::INSETS_TOP - Window::INSETS_BOTTOM 
				- getFontMetrics()->getHeight(this->titleString)) / 2;
			g->setColor(Color::WHITE);
			g->drawText(this->titleString, x, y);
			return;
		}
		
		// タイトル中の１文字を２倍に拡大して描画する
		if (getFontMetrics()->decodeCharacter(this->titleCharcode, &offset, &width, &height, fp) == true)
		{
			x = (getWidth() - Window::INSETS_LEFT - Window::INSETS_RIGHT - 24) / 2;
			y = (getHeight() - Window::INSETS_TOP - Window::INSETS_BOTTOM - 24) / 2;
			
			for (int j = 0; j < height; j++) {
				for (int k = 0; k < width; k++) {
					if ((fp[pos] & bit) != 0) {
						g->drawPixel(x + k * 2, y + j * 2, Color::WHITE);
						g->drawPixel(x + k * 2 + 1, y + j * 2, Color::WHITE);
						g->drawPixel(x + k * 2, y + j * 2 + 1, Color::WHITE);
						g->drawPixel(x + k * 2 + 1, y + j * 2 + 1, Color::WHITE);
					}
					bit <<= 1;
					if (bit == 256) {
						pos++;
						bit = 1;
					}
				}
			}
		}
	}
	
	/** イベントハンドラ */
	void onEvent(Event *event)
	{
		if (event->getType() == Event::TIMER) {
			drawTitle();
		}
	}
};

int MonaMain(List<char*>* pekoe)
{
	MbLupin *lupin = new MbLupin();
	lupin->run();
	delete(lupin);
	return 0;
}
