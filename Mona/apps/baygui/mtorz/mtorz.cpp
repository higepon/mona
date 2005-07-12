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

/* OSASKパレット */
static unsigned int osapal[] = {
	0xff000000,
	0xff800000,
	0xff008000,
	0xff808000,
	0xff000080,
	0xff800080,
	0xff008080,
	0xff808080,
	0xffc0c0c0,
};

static int col = 0;

class Mtorz : public Frame
{
public:
	Label* label1;

	Mtorz() {
		setBackground(osapal[8]);
		setBounds(200, 64, 14 * 8 + 12, 4 * 16 + 28);
		setTitle("M t . o r z 0");
		label1 = new Label(
			"　　　orz\n"
			"　　orz orz\n"
			"　orz orz orz\n"
			"orz orz orz orz");
		label1->setBounds(20, 8, 12 * 8, 3 * 16);
		label1->setBackground(osapal[8]);
		add(label1);
	}

	void paint(Graphics* g) {
		setTimer(500);
	}

	void processEvent(Event* e) {
		if (e->getType() == Event::TIMER) {
			label1->setForeground(osapal[col]);
			label1->repaint();
			if (++col > 7) {
				col = 0;
			}
			setTimer(500);
		}
	}
};

#if defined(MONA)
int MonaMain(List<char*>* pekoe) {
#else
int main(int argc, char** argv) {
#endif
	Mtorz *mtorz = new Mtorz();
	mtorz->run();
	delete(mtorz);
	return 0;
}
