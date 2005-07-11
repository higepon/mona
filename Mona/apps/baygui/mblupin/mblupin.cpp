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
			this->titleCharcode = s.charAt(i);
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
		setBounds((800 - 300) / 2,(600 - 200) / 2,300,200);
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
	void addNotify()
	{
		Window::addNotify();
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
		g->setColor(Color::black);
		g->fillRect(0,0,getWidth(),getHeight());
		
		// タイトル全部を表示
		if (this->titleCharcode == 0xFFFFFFFF) {
			x = (getWidth() - Window::INSETS_LEFT - Window::INSETS_RIGHT 
				- getFontMetrics()->getWidth(this->titleString)) / 2;
			y = (getHeight() - Window::INSETS_TOP - Window::INSETS_BOTTOM 
				- getFontMetrics()->getHeight(this->titleString)) / 2;
			g->setColor(Color::white);
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
						g->drawPixel(x + k * 2, y + j * 2, Color::white);
						g->drawPixel(x + k * 2 + 1, y + j * 2, Color::white);
						g->drawPixel(x + k * 2, y + j * 2 + 1, Color::white);
						g->drawPixel(x + k * 2 + 1, y + j * 2 + 1, Color::white);
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
