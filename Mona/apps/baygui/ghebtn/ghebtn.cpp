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

class HeButton : public Window {
private:
	int count;
	bool pushed;
	Image *number[10], *image_normal, *image_pushed;
	
public:
	HeButton(){
		setRect((800 - 191) / 2, (600 - 205) / 2, 191, 205);
		setTitle("へぇ～ボタン");
		count = 0;
		pushed = false;
		number[0] = new Image("/APPS/GHEBTN.APP/0.BM5");
		number[1] = new Image("/APPS/GHEBTN.APP/1.BM5");
		number[2] = new Image("/APPS/GHEBTN.APP/2.BM5");
		number[3] = new Image("/APPS/GHEBTN.APP/3.BM5");
		number[4] = new Image("/APPS/GHEBTN.APP/4.BM5");
		number[5] = new Image("/APPS/GHEBTN.APP/5.BM5");
		number[6] = new Image("/APPS/GHEBTN.APP/6.BM5");
		number[7] = new Image("/APPS/GHEBTN.APP/7.BM5");
		number[8] = new Image("/APPS/GHEBTN.APP/8.BM5");
		number[9] = new Image("/APPS/GHEBTN.APP/9.BM5");
		image_normal = new Image("/APPS/GHEBTN.APP/HENORMAL.BM5");
		image_pushed = new Image("/APPS/GHEBTN.APP/HEPUSHED.BM5");
	}
	
	~HeButton(){
		for (int i = 0; i < 10; i++) {
			delete(number[i]);
		}
		delete(image_normal);
		delete(image_pushed);
	}

	void onPaint(Graphics *g) {
		if (pushed == false) {
			g->drawImage(image_normal, 0, 0);
		} else {
			g->drawImage(image_pushed, 0, 0);
		}
		g->drawImage(number[count/10], 35, 112);
		g->drawImage(number[count%10], 65, 112);
	}
	
	void onEvent(Event *event) {
		if (event->type == MOUSE_PRESSED) {
			pushed = true;
			if (count == 20) {
				count = 0;
			} else {
				count++;
			}
			// 描画高速化
			repaint();
			//onPaint(__g);
		} else if (event->type  == MOUSE_RELEASED) {
			pushed = false;
			// 描画高速化
			repaint();
			//onPaint(__g);
		}
	}
};

int MonaMain(List<char*>* pekoe) {
	HeButton *hebtn = new HeButton();
	hebtn->run();
	return 0;
}
