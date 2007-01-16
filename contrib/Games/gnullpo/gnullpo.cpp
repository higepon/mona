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

class GNullpo : public Frame {
private:
	bool pushed;
	Image *image_normal, *image_pushed;
	
public:
	GNullpo(){
		setBounds((800 - 191) / 2, (600 - 205) / 2, 191, 205);
		setTitle("ぬるぽボタン");
		pushed = false;
		image_normal = new Image("/APPS/BAYGUI/GNULLPO.APP/NULLPO.BM5");
		image_pushed = new Image("/APPS/BAYGUI/GNULLPO.APP/GAT.BM5");
	}
	
	~GNullpo(){
		delete(image_normal);
		delete(image_pushed);
	}

	void paint(Graphics *g) {
		if (pushed == false) {
			g->drawImage(image_normal, 0, 0);
		} else {
			g->drawImage(image_pushed, 0, 0);
		}
	}
	
	void processEvent(Event *event) {
		if (event->getType() == MouseEvent::MOUSE_PRESSED) {
			pushed = true;
			repaint();
		} else if (event->getType()  == MouseEvent::MOUSE_RELEASED) {
			pushed = false;
			repaint();
		}
	}
};

int main(int argc, char* argv[]) {
	GNullpo *nullpo = new GNullpo();
	nullpo->run();
	return 0;
}
