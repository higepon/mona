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

#define SCREEN_W 160
#define SCREEN_H 100

extern "C" int DrawFire();

int typeDraw = 0, fireSeed = 0x1234;
unsigned char calc, *fireScreen, *pImage;

/** クラス宣言 */
class GFire : public Frame
{
private:
	bool firstPaint;

public:
	/** コンストラクタ */
	GFire() {
		setBounds((800 - SCREEN_W - 12) / 2, (600 - SCREEN_H - 28) / 2, SCREEN_W + 12, SCREEN_H + 28);
		setTitle("FIRE EFFECT");
		fireScreen = (unsigned char *)malloc(0x2300);
		pImage = (unsigned char *)malloc(SCREEN_W * SCREEN_H * 3);
		this->firstPaint = false;
	}

	/** デストラクタ */
	~GFire() {
		free(fireScreen);
		free(pImage);
	}

	/** 描画ハンドラ */
	virtual void paint(Graphics *g) {
		if (firstPaint == false) {
			firstPaint = true;
			MonAPI::Message::send(MonAPI::System::getThreadID(), Event::CUSTOM_EVENT, 0, 0, 0);
		}
	}
	
	/** イベント処理 */
	virtual void processEvent(Event *e) {
		if (e->getType() == Event::CUSTOM_EVENT) {
			if (DrawFire() != 0) {
				for (int y = 0; y < SCREEN_H; y++) {
					for (int x = 0; x < SCREEN_W; x++) {
						int k = (x + y * SCREEN_W) * 3;
						unsigned char r = pImage[k];
						unsigned char g = pImage[k + 1];
						unsigned char b = pImage[k + 2];
						//this->_g->drawPixel(x, y, 0xff000000 | r << 16 | g << 8 | b);
						getBuffer()->setPixel(x, y, 0xff000000 | r << 16 | g << 8 | b);
					}
				}
				update();
			}
			MonAPI::Message::send(MonAPI::System::getThreadID(), Event::CUSTOM_EVENT, 0, 0, 0);
		}
	}
};

int MonaMain(List<char*>* pekoe) {
	GFire *fire = new GFire();
	fire->run();
	delete(fire);
	return 0;
}
