/*
Copyright (c) 2004 Tino, bayside
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

extern dword __gui_server;
extern CommonParameters* __commonParams;

#define BASE Control

/** closeIcon (palette) */
static unsigned int close_palette[] = {
	0xff1c1a1c,
	0xff8c8e8c,
	0xffcccecc,
	0xffacaeac,
	0xffeceeec,
	0xff9c9e9c,
	0xffdcdedc,
	0xffbcbebc,
	0xfffcfefc,
};

/** closeIcon (data) */
static unsigned char close_data[] = {
	0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x2,
	0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x8,
	0x1,0x0,0x8,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x0,0x8,
	0x1,0x0,0x2,0x5,0x5,0x3,0x3,0x7,0x7,0x2,0x1,0x0,0x8,
	0x1,0x0,0x2,0x5,0x3,0x3,0x7,0x7,0x2,0x2,0x1,0x0,0x8,
	0x1,0x0,0x2,0x3,0x3,0x7,0x7,0x2,0x2,0x6,0x1,0x0,0x8,
	0x1,0x0,0x2,0x3,0x7,0x7,0x2,0x2,0x6,0x6,0x1,0x0,0x8,
	0x1,0x0,0x2,0x7,0x7,0x2,0x2,0x6,0x6,0x4,0x1,0x0,0x8,
	0x1,0x0,0x2,0x7,0x2,0x2,0x6,0x6,0x4,0x4,0x1,0x0,0x8,
	0x1,0x0,0x2,0x2,0x2,0x6,0x6,0x4,0x4,0x8,0x1,0x0,0x8,
	0x1,0x0,0x2,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x0,0x8,
	0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x8,
	0x2,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,
};

namespace baygui
{
	Window::Window() : isCloseButtonPushed(false), ncState(NCState_None), opacity(1.0)
	{
		this->offset = Point(6, 22);
		this->overlap = 0;
	}
	
	Window::~Window()
	{
		this->onExit();
	}
	
	void Window::setTitle(const char* title)
	{
		//if (this->text != NULL) delete[] this->text;
		//this->text = new char[strlen(text) + 1];
		//strcpy(this->text, text);
		this->title = title;
		if (this->buffer == NULL) return;
		this->repaint();
	}
	
	void Window::onHide()
	{
		if (!this->getVisible()) return;
		
		BASE::onHide();
		this->onErase();
	}
	
	void Window::onStart()
	{
		BASE::onStart();
		Application::addWindow(this);
		this->isCloseButtonPushed = false;
		this->ncState = NCState_None;
		this->formBuffer = new Bitmap(this->getWidth(), this->getHeight());
		this->_object->FormBufferHandle = this->formBuffer->getHandle();
		this->_object->Opacity = (int)(this->opacity * 255.0);
	}
	
	void Window::onExit()
	{
		BASE::onExit();
		Application::removeWindow(this);
		this->formBuffer = NULL;
	}
	
	void Window::onErase()
	{
		if (this->buffer == NULL) return;
		
		this->_object->Visible = false;
		::monapi_call_mouse_set_cursor(MONAPI_FALSE);
		MonAPI::Message::sendReceive(NULL, __gui_server, MSG_GUISERVER_DRAWWINDOW, this->getHandle());
		::monapi_call_mouse_set_cursor(MONAPI_TRUE);
		this->_object->Visible = this->getVisible();
	}
	
	void Window::drawInternal()
	{
		if (this->buffer == NULL || (this->_object->Flags & WINDOWFLAGS_NOBORDER) != 0) return;
		
		_P<Graphics> g = Graphics::getGraphics(this->buffer);
		int w = this->getWidth(), h = this->getHeight();
		
		// 外枠
		g->setColor(COLOR_LIGHTGRAY);
		g->fillRect(0, 0, w, h);
		g->setColor(COLOR_BLACK);
		g->drawRect(0, 0, w, h);
		
		// 内枠
		g->setColor(COLOR_BLACK);
		g->drawRect(5, 21, w - 10, h - 26);
		
		// 枠
		g->setColor(COLOR_WHITE);
		g->drawLine(1, 1, w - 2, 1);
		g->drawLine(1, 1, 1, h - 2);
		g->drawLine(w - 5, 21, w - 5, h - 5);
		g->drawLine(5, h - 5, w - 5, h - 5);
		g->setColor(COLOR_GRAY);
		g->drawLine(w - 2, 2, w - 2, h - 2);
		g->drawLine(2, h - 2, w - 2, h - 2);
		g->drawLine(4, 20, w - 6, 20);
		g->drawLine(4, 20, 4, h - 6);
		
		// タイトルバー
		for (int i = 4; i <= 14; i = i + 2) {
			g->setColor(COLOR_GRAY);
			g->drawLine(20, i, w - 7, i);
			g->setColor(COLOR_WHITE);
			g->drawLine(21, i + 1, w - 6, i + 1);
		}
		
		// 閉じるボタン
		for (int i = 0; i < 13; i++) {
			for (int j = 0; j < 13; j++) {
				g->drawPixel(j + 4, i + 4, close_palette[close_data[i * 13 + j] & 0xFF]);
			}
		}
		
		// タイトル
		FontMetrics* metrics = FontMetrics::getFontMetrics();
		int fw = metrics->getWidth(this->getTitle());
		int fh = metrics->getHeight(this->getTitle());
		g->setColor(COLOR_LIGHTGRAY);
		g->fillRect(((w - fw) / 2) - 4, 2, fw + 8, 22 - 4);
		g->setColor(COLOR_BLACK);
		g->drawText(this->getTitle(), ((w - fw) / 2), ((22 - fh) / 2));
		//g->drawText(this->getTitle(), ((w - fw) / 2) + 1, ((22 - fh) / 2));
		
		g->dispose();
	}
	
	void Window::postEvent(Event *e)
	{
		if (e->type == MOUSE_MOVED) {
			MouseEvent *me = (MouseEvent *)e;
			Point pt = (me->button == 0 ? Point(me->x, me->y) : this->clickPoint);
			
			// 閉じるボタンドラッグ
			if (this->ncState == NCState_CloseButton)
			{
				bool pushed = (this->NCHitTest(pt.X, pt.Y) == NCState_CloseButton);
				if (this->isCloseButtonPushed != pushed) {
					this->isCloseButtonPushed = pushed;
					this->repaint();
				}
			}
			// タイトルバードラッグ（移動中）
			else if (this->ncState == NCState_TitleBar)
			{
				Point p = this->pointToClient(Point(__commonParams->mouse.x, __commonParams->mouse.y));
				//Point p = this->pointToClient(Cursor::getPosition());
				int ex = this->getX() + (p.X - this->clickPoint.X), ey = this->getY() + (p.Y - this->clickPoint.Y);
				if (this->ptRevRect.X != ex || this->ptRevRect.Y != ey) {
					this->ptRevRect = Point(ex, ey);
					MonAPI::Message::sendReceive(NULL, __gui_server, MSG_GUISERVER_MOVEOVERLAP, this->overlap,
						MAKE_DWORD(ex, ey), MAKE_DWORD(this->getWidth(), this->getHeight()));
				}
			}
			// 内部領域ドラッグ
			else if (this->NCHitTest(pt.X, pt.Y) == NCState_Client)
			{
				this->onEvent(e);
			}
		} else if (e->type == MOUSE_PRESSED) {
			MouseEvent *me = (MouseEvent *)e;
			this->clickPoint = Point(me->x, me->y);
			this->ncState = this->NCHitTest(me->x, me->y);
			
			// 閉じるボタン押下
			if (this->ncState == NCState_CloseButton)
			{
				this->setFocused(true);
				this->isCloseButtonPushed = true;
				this->repaint();
			}
			// タイトルバー押下（移動開始）
			else if (this->ncState == NCState_TitleBar)
			{
				this->setFocused(true);
				this->ptRevRect = Point(me->x, me->y);
				MessageInfo msg;
				MonAPI::Message::sendReceive(&msg, __gui_server, MSG_GUISERVER_CREATEOVERLAP,
					this->getX(), this->getY(),
					MAKE_DWORD(this->getWidth(), this->getHeight()));
				this->overlap = msg.arg2;
			}
			// 内部領域押下
			else if (this->ncState == NCState_Client)
			{
				this->onEvent(e);
			}
		} else if(e->type == MOUSE_RELEASED) {
			MouseEvent *me = (MouseEvent *)e;
			bool destroy = (this->NCHitTest(me->x, me->y) == NCState_CloseButton) &&
								(this->ncState == NCState_CloseButton);
			
			// 閉じるボタンリリース
			if (this->ncState == NCState_CloseButton)
			{
				this->setFocused(false);
				this->isCloseButtonPushed = false;
			}
			// タイトルバーリリース（移動終了）
			else if (this->ncState == NCState_TitleBar)
			{
				MonAPI::Message::sendReceive(NULL, __gui_server, MSG_GUISERVER_DISPOSEOVERLAP, this->overlap);
				this->overlap = 0;
				this->setFocused(false);
				Point p = this->getLocation();
				p.X += me->x - this->clickPoint.X;
				p.Y += me->y - this->clickPoint.Y;
				this->setLocation(p.X, p.Y);
			}
			// 内部領域リリース
			else if (this->ncState == NCState_Client)
			{
				this->onEvent(e);
			}
			// それ以外
			else
			{
				this->repaint();
			}
			
			this->ncState = NCState_None;
			if (destroy) this->onExit();
		} else {
			this->onEvent(e);
		}
	}
	
	void Window::run()
	{
		Application::initialize();
		Application::run(this);
		Application::dispose();
	}
	
	Control::NCState Window::NCHitTest(int x, int y)
	{
		if ((this->_object->Flags & WINDOWFLAGS_NOBORDER) != 0 ||
			BASE::NCHitTest(x, y) == NCState_Client)
		{
			return NCState_Client;
		}
		
		int oy = this->offset.Y, xx = x + this->offset.X, yy = y + oy;
		if (Rect(4, 4, oy - 5, oy - 5).Contains(xx, yy)) return NCState_CloseButton;
		if (yy < oy) return NCState_TitleBar;
		return NCState_None;
	}
}
