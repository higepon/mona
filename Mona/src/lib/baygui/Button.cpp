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

#define BASE Control

namespace baygui
{
	Button::Button() : isPushed(false)
	{
	}
	
	Button::~Button()
	{
	}
	
	void Button::onStart()
	{
		BASE::onStart();
		this->isPushed = false;
	}
	
	void Button::onPaint(_P<Graphics> g)
	{
		int w = this->getWidth(), h = this->getHeight();
		
		// 一度背景色でクリア
		g->setColor(this->getBackground());
		g->fillRect(0, 0, w, h);
		
		// 枠を描画
		g->setColor(COLOR_BLACK);
		g->drawLine(2, 0, w - 3, 0);
		g->drawLine(2, h - 1, w - 3, h - 1);
		g->drawLine(0, 2, 0, h - 3);
		g->drawLine(w - 1, 2, w - 1, h - 3);
		g->drawLine(1, 1, 1, 1);
		g->drawLine(1, h - 2 , 1, h - 2);
		g->drawLine(w - 2 , 1, w - 2, 1);
		g->drawLine(w - 2 , h - 2, w - 2, h - 2);
		if (this->isPushed) {
			g->setColor(COLOR_WHITE);
			g->drawLine(2, h - 2, w - 3, h - 2);
			g->drawLine(w - 2, 2, w - 2, h - 3);
			g->drawLine(w - 3 , h - 3, w - 3, h - 3);
			g->setColor(COLOR_GRAY);
			g->drawLine(1, 2, 1, h - 3);
			g->drawLine(2, 1, w - 3, 1);
		} else {
			g->setColor(COLOR_GRAY);
			g->drawLine(2, h - 2, w - 3, h - 2);
			g->drawLine(w - 2, 2, w - 2, h - 3);
			g->drawLine(w - 3 , h - 3, w - 3, h - 3);
			g->setColor(COLOR_WHITE);
			g->drawLine(1, 2, 1, h - 3);
			g->drawLine(2, 1, w - 3, 1);
		}
		
		// テキストを描画
		FontMetrics *manager = FontMetrics::getFontMetrics();
		int fw = manager->getWidth(this->getText());
		int fh = manager->getHeight(this->getText());
		int x = (w - fw) / 2;
		int y = (h - fh) / 2;
		if (this->isPushed) {
			x++;
			y++;
		}
		g->setColor(this->getForeground());
		g->drawText(this->getText(), x, y);
	}
	
	void Button::onEvent(Event *_e)
	{
		switch (_e->type)
		{
			// マウスが動いたとき
			case MOUSE_MOVED:
			{
				MouseEvent *e = (MouseEvent *)_e;
				
				if (e->button != 0) {
					bool pushed = Rect(Point::get_Empty(), this->getClientSize()).Contains(e->x, e->y);
					if (this->isPushed != pushed) {
						this->isPushed = pushed;
						this->repaint();
					}
				}
				
				BASE::onEvent(e);
				break;
			}
			// マウスが押されたとき
			case MOUSE_PRESSED:
			{
				MouseEvent *e = (MouseEvent *)_e;
				
				this->setFocused(true);
				this->isPushed = true;
				this->repaint();
				
				BASE::onEvent(e);
				break;
			}
			// マウスが離されたとき
			case MOUSE_RELEASED:
			{
				MouseEvent *e = (MouseEvent *)_e;
				
				this->setFocused(false);
				if (this->isPushed) {
					this->isPushed = false;
					this->repaint();
				}
				
				BASE::onEvent(e);
				break;
			}
			default:
				break;
		}
	}
}
