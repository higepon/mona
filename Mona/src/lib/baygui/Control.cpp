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

static _P<MonAPI::Screen> s_screen;

_P<MonAPI::Screen> getDefaultScreen()
{
	if (s_screen == NULL) {
		s_screen.Set(new MonAPI::Screen(), true);
	}
	return s_screen;
}

#if 0
static Rect getScreenRect()
{
	_P<MonAPI::Screen> scr = getDefaultScreen();
	return Rect(0, 0, scr->getWidth(), scr->getHeight());
}
#endif

void drawImage(_P<Bitmap> dst, _P<Bitmap> src, int x, int y, int sx, int sy, int sw, int sh, bool srccopy)
{
	int dw = dst->getWidth(), dh = dst->getHeight();
	int x1 = x, y1 = y, x2 = x + sw, y2 = y + sh;
	if (x1 < 0) x1 = 0;
	if (y1 < 0) y1 = 0;
	if (x2 > dw) x2 = dw;
	if (y2 > dh) y2 = dh;
	x1 += sx - x;
	y1 += sy - y;
	x2 += sx - x;
	y2 += sy - y;
	if (x1 < 0) x1 = 0;
	if (y1 < 0) y1 = 0;
	if (x2 > src->getWidth ()) x2 = src->getWidth ();
	if (y2 > src->getHeight()) y2 = src->getHeight();
	x1 -= sx;
	y1 -= sy;
	x2 -= sx;
	y2 -= sy;
	for (int yy = y1; yy < y2; yy++) {
		unsigned int* pDst = &dst->get()[( x + x1) + ( y + yy) * dw];
		unsigned int* pSrc = &src->get()[(sx + x1) + (sy + yy) * src->getWidth()];
		for (int xx = x1; xx < x2; xx++, pDst++, pSrc++) {
			// 透過色は描画しない
			if (srccopy || getAlpha(*pSrc) != 0) *pDst = *pSrc;
		}
	}
}

namespace baygui
{
	Control::Control()
	{
		this->rect = Rect(0, 0, 32, 32);
		this->foreColor = DEFAULT_FORECOLOR;
		this->backColor = DEFAULT_BACKCOLOR;
		this->foreColorChanged = false;
		this->backColorChanged = false;
		this->visible = false;
		this->focused = false;
		this->transColor = DEFAULT_TRANSCOLOR;
		this->_object = NULL;
		this->children = new ControlCollection();
		this->children->target = this;
	}
	
	Control::~Control()
	{
		this->onExit();
	}
	
	void Control::onShow()
	{
		if (this->visible) return;
		
		if (this->buffer == NULL) this->onStart();
		this->visible = true;
		this->_object->Visible = this->visible;
		this->repaint();
	}
	
	void Control::onHide()
	{
		if (!this->visible) return;
		
		this->visible = false;
		if (this->parent != NULL) this->parent->repaint();
	}
	
	void Control::onStart()
	{
		if (this->buffer != NULL) return;
		
		this->buffer = new Bitmap(this->getWidth(), this->getHeight());
		MessageInfo msg;
		if (MonAPI::Message::sendReceive(&msg, __gui_server, MSG_GUISERVER_CREATEWINDOW) != 0) {
			::printf("%s:%d:ERROR: can not connect to GUI server!\n", __FILE__, __LINE__);
			this->onExit();
			return;
		}
		this->_object = (guiserver_window*)MonAPI::MemoryMap::map(msg.arg2);
		if (this->_object == NULL) {
			::printf("%s:%d:ERROR: can not create window!\n", __FILE__, __LINE__);
			this->onExit();
			return;
		}
		if (this->parent != NULL) this->_object->Parent = this->parent->getHandle();
		this->_object->X = this->getX();
		this->_object->Y = this->getY();
		this->_object->Width  = this->getWidth();
		this->_object->Height = this->getHeight();
		this->_object->OffsetX = this->offset.X;
		this->_object->OffsetY = this->offset.Y;
		this->_object->BufferHandle = this->buffer->getHandle();
		this->_object->TransparencyKey = this->transColor;
		//int len = this->getWidth() * this->getHeight();
		//for (int i = 0; i < len; i++) {
		//	(*this->buffer)[i] = this->backColor;
		//}
		if (this->parent != NULL) {
			if (!this->foreColorChanged) this->foreColor = this->parent->foreColor;
			if (!this->backColorChanged) this->backColor = this->parent->backColor;
		}
		
		FOREACH_AL(_P<Control>, ctrl, this->children) {
			ctrl->onStart();
		}
		END_FOREACH_AL
	}
	
	void Control::onExit()
	{
		this->onHide();
		
		MonAPI::Message::sendReceive(NULL, __gui_server, MSG_GUISERVER_DISPOSEWINDOW, this->getHandle());
		this->_object = NULL;
		this->parent = NULL;
		this->buffer = NULL;
		
		FOREACH_AL(_P<Control>, ctrl, this->children) {
			ctrl->onExit();
		}
		END_FOREACH_AL
		this->children->Clear();
		//this->children->target = NULL;
	}
	
	void Control::repaint()
	{
		if (this->buffer == NULL) return;
		
		this->repaintInternal();
		MonAPI::Message::sendReceive(NULL, __gui_server, MSG_GUISERVER_DRAWWINDOW, this->getTopLevelControl()->getHandle());
	}
	
	void Control::repaintInternal()
	{
		if (this->buffer == NULL) return;
		
		Rect r(0, 0, this->getWidth(), this->getHeight());
		int x = 0, y = 0;
		_P<Control> form;
		for (_P<Control> c = this; c != NULL; c = c->parent) {
			if (!c->visible) return;
			
			if (c->parent != NULL) {
				r.X += c->getX();
				r.Y += c->getY();
				r.Intersect(Rect(Point(), c->parent->getInnerSize()));
				r.X += c->parent->offset.X;
				r.Y += c->parent->offset.Y;
				x += c->getX() + c->parent->offset.X;
				y += c->getY() + c->parent->offset.Y;
			}
			form = c;
		}
		
		drawInternal();
		
		_P<Graphics> g = getGraphics();
		this->onPaint(g);
		g->dispose();
		
		drawImage(((Window*)form.get())->formBuffer, this->buffer, r.X, r.Y, r.X - x, r.Y - y, r.Width, r.Height, this->parent == NULL);
		FOREACH_AL(_P<Control>, ctrl, this->children) {
			ctrl->repaintInternal();
		}
		END_FOREACH_AL
	}
	
	_P<Graphics> Control::getGraphics()
	{
		_P<Graphics> ret = Graphics::getGraphics(this->buffer);
		Dimention sz = this->getInnerSize();
		ret->setClientRect(Rect(this->offset.X, this->offset.Y, sz.Width, sz.Height));
		return ret;
	}
	
	Point Control::pointToClient(Point p)
	{
		p -= this->getLocation() + this->offset;
		return this->parent == NULL ? p : this->parent->pointToClient(p);
	}
	
	Point Control::pointToScreen(Point p)
	{
		p += this->getLocation() + this->offset;
		return this->parent == NULL ? p : this->parent->pointToScreen(p);
	}
	
	_P<Control> Control::getTopLevelControl()
	{
		return this->parent == NULL ? this : this->parent->getTopLevelControl();
	}
	
	_P<Control> Control::findChild(int x, int y)
	{
		if (!this->rect.Contains(x, y)) return NULL;
		
		x -= this->getX();
		y -= this->getY();
		if (getAlpha(this->buffer->getPixel(x, y)) == 0) return NULL;
		
		x -= this->offset.X;
		y -= this->offset.Y;
		Dimention sz = this->getInnerSize();
		if (x >= sz.Width || y >= sz.Height) return this;
		
		FOREACH_AL(_P<Control>, c, this->children) {
			_P<Control> fc = c->findChild(x, y);
			if (fc != NULL) return fc;
		}
		END_FOREACH_AL
		return this;
	}
	
	void Control::setLocation(int x, int y)
	{
		if (this->getX() == x && this->getY() == y) return;
		
		this->rect.X = x;
		this->rect.Y = y;
		if (this->_object == NULL) return;
		
		if (this->parent == NULL) {
			MonAPI::Message::sendReceive(NULL, __gui_server, MSG_GUISERVER_MOVEWINDOW, this->getHandle(), (dword)x, (dword)y);
		} else {
			this->_object->X = x;
			this->_object->Y = y;
			this->parent->repaint();
		}
	}
	
	Dimention Control::getInnerSize()
	{
		int bw = this->offset.X;
		return Dimention(this->getWidth() - bw * 2, this->getHeight() - this->offset.Y - bw);
	}
	
	void Control::setInnerSize(int width, int height)
	{
		int bw = this->offset.X;
		this->setSize(width + bw * 2, height + this->offset.Y + bw);
	}
	
	void Control::setVisible(bool v)
	{
		if (v) this->onShow(); else this->onHide();
	}
	
	void Control::setFocused(bool v)
	{
		if (v == this->focused) return;
		
		this->focused = v;
		MonAPI::Message::sendReceive(NULL, __gui_server, MSG_GUISERVER_MOUSECAPTURE, this->getHandle(), v ? 1 : 0);
	}
	
	void Control::setForeground(unsigned int c)
	{
		if (this->foreColor == c) return;
		
		this->foreColor = c;
		this->foreColorChanged = true;
		//this->OnForeColorChanged(EventArgs::get_Empty());
		if (this->buffer == NULL) return;
		
		this->repaint();
	}
	
	void Control::setBackground(unsigned int c)
	{
		if (this->backColor == c) return;
		
		this->backColor = c;
		this->backColorChanged = true;
		//this->OnBackColorChanged(EventArgs::get_Empty());
		if (this->buffer == NULL) return;
		
		this->repaint();
	}
	
	void Control::setTransColor(unsigned int c)
	{
		this->transColor = c;
		if (this->_object == NULL) return;
		
		//unsigned int v = c.getArgb();
		if (this->_object->TransparencyKey == c) return;
		
		this->_object->TransparencyKey = c;
		this->repaint();
	}
	
	Control::NCState Control::NCHitTest(int x, int y)
	{
		return Rect(Point::get_Empty(), this->getInnerSize()).Contains(x, y)
			? NCState_Client : NCState_None;
	}
	
	void Control::postEvent(Event *e)
	{
		if (e->type == MOUSE_MOVED) {
			MouseEvent *me = (MouseEvent *)e;
			Point pt = me->button == 0 ? Point(me->x, me->y) : this->clickPoint;
			if (this->NCHitTest(pt.X, pt.Y) == NCState_Client) {
				this->onEvent(e);
			}
		} else if (e->type == MOUSE_PRESSED) {
			MouseEvent *me = (MouseEvent *)e;
			this->clickPoint = Point(me->x, me->y);
			if (this->NCHitTest(me->x, me->y) == NCState_Client) {
				this->onEvent(e);
			}
		} else if (e->type == MOUSE_RELEASED) {
			if (this->NCHitTest(this->clickPoint.X, this->clickPoint.Y) == NCState_Client) {
				this->onEvent(e);
			}
		} else {
			this->onEvent(e);
		}
	}
	
	void Control::add(_P<Control> control)
	{
		this->children->Add(control);
		control->parent = this;
		control->visible = true;
	}
}
