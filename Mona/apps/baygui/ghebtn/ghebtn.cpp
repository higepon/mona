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
	Bitmap *number[10], *image_normal, *image_pushed;
	
public:
	HeButton(){
		setRect(220, 140, 180 + 11, 178 + 27);
		setTitle("へぇ～ボタン");
		count = 0;
		pushed = false;
		number[0] = new Bitmap("/APPS/GHEBTN.APP/0.BM2");
		number[1] = new Bitmap("/APPS/GHEBTN.APP/1.BM2");
		number[2] = new Bitmap("/APPS/GHEBTN.APP/2.BM2");
		number[3] = new Bitmap("/APPS/GHEBTN.APP/3.BM2");
		number[4] = new Bitmap("/APPS/GHEBTN.APP/4.BM2");
		number[5] = new Bitmap("/APPS/GHEBTN.APP/5.BM2");
		number[6] = new Bitmap("/APPS/GHEBTN.APP/6.BM2");
		number[7] = new Bitmap("/APPS/GHEBTN.APP/7.BM2");
		number[8] = new Bitmap("/APPS/GHEBTN.APP/8.BM2");
		number[9] = new Bitmap("/APPS/GHEBTN.APP/9.BM2");
		image_normal = new Bitmap("/APPS/GHEBTN.APP/HENORMAL.BM2");
		image_pushed = new Bitmap("/APPS/GHEBTN.APP/HEPUSHED.BM2");
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
			repaint();
		} else if (event->type  == MOUSE_RELEASED) {
			pushed = false;
			repaint();
		}
	}
};

int MonaMain(List<char*>* pekoe) {
	HeButton *hebtn = new HeButton();
	hebtn->run();
	return 0;
}
