/*
Copyright (c) 2004 bayside
All rights reserved.

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

class HeButton : public Window
{
private:
	int count;
	bool pushed;
	_P<Bitmap> number[10];
	_P<Bitmap> image_normal;
	_P<Bitmap> image_pushed;
	
public:
	HeButton()
	{
		setRect((800 - 180 - 12) / 2, (600 - 178 - 28) / 2, 192, 206);
		setTitle("へぇ～ボタン");
	}
	
	~HeButton(){
		//for (int i = 0; i < 10; i++) {
		//	delete(number[i]);
		//}
		//delete(image_normal);
		//delete(image_pushed);
	}
	
	void onStart()
	{
		Window::onStart();
		count = 0;
		pushed = false;
		number[0] = new Bitmap("/APPS/GHEBTN.APP/0.BM5");
		number[1] = new Bitmap("/APPS/GHEBTN.APP/1.BM5");
		number[2] = new Bitmap("/APPS/GHEBTN.APP/2.BM5");
		number[3] = new Bitmap("/APPS/GHEBTN.APP/3.BM5");
		number[4] = new Bitmap("/APPS/GHEBTN.APP/4.BM5");
		number[5] = new Bitmap("/APPS/GHEBTN.APP/5.BM5");
		number[6] = new Bitmap("/APPS/GHEBTN.APP/6.BM5");
		number[7] = new Bitmap("/APPS/GHEBTN.APP/7.BM5");
		number[8] = new Bitmap("/APPS/GHEBTN.APP/8.BM5");
		number[9] = new Bitmap("/APPS/GHEBTN.APP/9.BM5");
		image_normal = new Bitmap("/APPS/GHEBTN.APP/HENORMAL.BM5");
		image_pushed = new Bitmap("/APPS/GHEBTN.APP/HEPUSHED.BM5");
	}
	
	void onPaint(_P<Graphics> g)
	{
		if (pushed == false) {
			g->drawImage(image_normal, 0, 0);
		} else {
			g->drawImage(image_pushed, 0, 0);
		}
		g->drawImage(number[count/10], 35, 112);
		g->drawImage(number[count%10], 65, 112);
	}
	
	void onEvent(Event *e) {
		if (e->type == MOUSE_PRESSED) {
			pushed = true;
			if (count == 20) {
				count = 0;
			} else {
				count++;
			}
			repaint();
		} else if (e->type  == MOUSE_RELEASED) {
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
