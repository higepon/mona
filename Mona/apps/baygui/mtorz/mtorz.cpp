/*
Copyright (c) 2004 Akkie, bayside
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

class Mtorz : public Window
{
public:
	_P<Label> label1;
	_P<Timer> timer1;

	Mtorz()
	{
		label1 = new Label();

		setBackground(osapal[8]);
		/* ウィンドウの位置＜Point(Xpos, Ypos)＞ */
		setLocation(200, 64);
		/* ウィンドウのサイズ。＜Size(Xsize, Ysize)＞ */
		setClientSize(14 * 8, 4 * 16);
		setText("M t . o r z 0");

		/* ＜Rect(Xpos, Ypos, Xsize, Ysize)＞ */
		label1->setRect(20, 8, 12 * 8, 3 * 16);
		label1->setBackground(osapal[8]);
		label1->setText(
			"　　　orz\n"
			"　　orz orz\n"
			"　orz orz orz\n"
			"orz orz orz orz");

		add(label1.get());

		timer1 = new Timer();
		timer1->setInterval(500);
		ChangeColor();
	}

	void onEvent(Event *e)
	{
		if (e->type == TIMER) {
			ChangeColor();
		}
		Window::onEvent(e);
	}

	virtual void onStart()
	{
		Window::onStart();
		timer1->start();
	}

	virtual void onExit()
	{
		timer1->dispose();
		Window::onExit();
	}

private:
	void ChangeColor()
	{
		label1->setForeground(osapal[col]);
		if (++col > 7) {
			col = 0;
		}
	}
};

int MonaMain(List<char*>* pekoe) {
	Mtorz *mtorz = new Mtorz();
	mtorz->run();
	return 0;
}
