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

class GNullpo : public Window
{
private:
	bool pushed;
	_P<Bitmap> image_normal;
	_P<Bitmap> image_pushed;
	
public:
	GNullpo()
	{
		setLocation((800 - 180 - 12) / 2, (600 - 178 - 28) / 2);
		setClientSize(180,178);
		setText("ぬるぽボタン");
	}
	
	~GNullpo()
	{
		//delete(image_normal);
		//delete(image_pushed);
	}

	void onStart()
	{
		Window::onStart();
		pushed = false;
		image_normal = new Bitmap("/APPS/GNULLPO.APP/NULLPO.BM5");
		image_pushed = new Bitmap("/APPS/GNULLPO.APP/GAT.BM5");
	}

	void onPaint(_P<Graphics> g)
	{
		if (pushed == false) {
			g->drawImage(image_normal, 0, 0);
		} else {
			g->drawImage(image_pushed, 0, 0);
		}
	}
	
	void onEvent(Event *e)
	{
		#if 1
		if (e->type == WM_MOUSEDOWN) {
			pushed = true;
			update();
		} else if (e->type  == WM_MOUSEUP) {
			pushed = false;
			update();
		}
		#endif
		Window::onEvent(e);
	}
};

int MonaMain(List<char*>* pekoe) {
	GNullpo *nullpo = new GNullpo();
	nullpo->run();
	return 0;
}
