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

static unsigned int color4bit[16] = {
	0xff000000,
	0xff800000,
	0xff008000,
	0xff808000,
	0xff000080,
	0xff800080,
	0xff008080,
	0xffc0c0c0,
	0xff808080,
	0xffff0000,
	0xff00ff00,
	0xffffff00,
	0xff0000ff,
	0xffff00ff,
	0xff00ffff,
	0xffffffff,
};

static int point[16][2] = {
	{ 196, 100 }, { 187,  61 }, { 164,  29 }, { 129,   9 }, {  90,   5 },
	{  53,  17 }, {  23,  44 }, {   7,  81 }, {   7, 119 }, {  23, 156 },
	{  53, 183 }, {  90, 195 }, { 129, 191 }, { 164, 171 }, { 187, 139 },
	{ 196, 100 }
};

class GBBall : public Window
{
public:
	GBBall()
	{
		setLocation((800 - 200 - 12) / 2, (600 - 200 - 28) / 2);
		setClientSize(200, 200);
		setText("bball");
	}
	
	void onPaint(_P<Graphics> g)
	{
		int i, j;
		Window::onPaint(g);
		g->setColor(0, 0, 0);
		g->fillRect(0, 0, 200, 200);
		for (i = 0; i <= 14; i++) {
			int x0, y0, dis;
			x0 = point[i][0];
			y0 = point[i][1];
			for (j = i + 1; j <= 15; j++) {
				dis = j - i; /* 2‚Â‚Ì“_‚Ì‹——£ */
				if (dis >= 8)
				dis = 15 - dis; /* ‹t‰ñ‚è‚É”‚¦‚é */
				if (dis != 0) {
					g->setColor(color4bit[16 - dis]);
					g->drawLine(x0, y0, point[j][0], point[j][1]);
				}
			}
		}
	}
};

int MonaMain(List<char*>* pekoe) {
	GBBall *bball = new GBBall();
	bball->run();
	return 0;
}
