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

class HeButton : public Frame {
private:
	int count;
	bool pushed;
	Image *number[10], *image_normal, *image_pushed;
	
public:
	HeButton(){
		setBounds((800 - 191) / 2, (600 - 205) / 2, 191, 205);
		setTitle("へぇ～ボタン");
		count = 0;
		pushed = false;
		number[0] = new Image("/APPS/BAYGUI/GHEBTN.APP/0.BM5");
		number[1] = new Image("/APPS/BAYGUI/GHEBTN.APP/1.BM5");
		number[2] = new Image("/APPS/BAYGUI/GHEBTN.APP/2.BM5");
		number[3] = new Image("/APPS/BAYGUI/GHEBTN.APP/3.BM5");
		number[4] = new Image("/APPS/BAYGUI/GHEBTN.APP/4.BM5");
		number[5] = new Image("/APPS/BAYGUI/GHEBTN.APP/5.BM5");
		number[6] = new Image("/APPS/BAYGUI/GHEBTN.APP/6.BM5");
		number[7] = new Image("/APPS/BAYGUI/GHEBTN.APP/7.BM5");
		number[8] = new Image("/APPS/BAYGUI/GHEBTN.APP/8.BM5");
		number[9] = new Image("/APPS/BAYGUI/GHEBTN.APP/9.BM5");
		image_normal = new Image("/APPS/BAYGUI/GHEBTN.APP/HENORMAL.BM5");
		image_pushed = new Image("/APPS/BAYGUI/GHEBTN.APP/HEPUSHED.BM5");
	}
	
	~HeButton(){
		for (int i = 0; i < 10; i++) {
			delete(number[i]);
		}
		delete(image_normal);
		delete(image_pushed);
	}

	void paint(Graphics *g) {
		if (pushed == false) {
			g->drawImage(image_normal, 0, 0);
		} else {
			g->drawImage(image_pushed, 0, 0);
		}
		g->drawImage(number[count/10], 35, 112);
		g->drawImage(number[count%10], 65, 112);
	}
	
	void processEvent(Event *event) {
		if (event->getType() == MouseEvent::MOUSE_PRESSED) {
			pushed = true;
			if (count == 20) {
				count = 0;
			} else {
				count++;
			}
			repaint();
		} else if (event->getType()  == MouseEvent::MOUSE_RELEASED) {
			pushed = false;
			repaint();
		}
	}
};

int main(int argc, char* argv[]) {
	HeButton *hebtn = new HeButton();
	hebtn->run();
	return 0;
}
