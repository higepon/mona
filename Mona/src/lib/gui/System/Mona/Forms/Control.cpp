// This file is in the public domain.
// There are no restrictions on any sort of usage of this file.

#ifdef MONA
#include <monapi.h>
#include <monapi/messages.h>

extern dword __gui_server;
#endif

#include <gui/System/Mona/Forms/Application.h>
#include <gui/System/Mona/Forms/Control.h>
#include <gui/System/Mona/Forms/ControlPaint.h>
#include <gui/System/Mona/Forms/Form.h>

using namespace System;
using namespace System::Collections;
using namespace System::Drawing;

#define DEFAULT_FORECOLOR Color::get_Black()
#define DEFAULT_BACKCOLOR ControlPaint::get_Light()

#ifdef MONA
#include <map>
std::map<dword, System::Mona::Forms::Control*> mapControls;

static _P<MonAPI::Screen> s_screen;

_P<MonAPI::Screen> GetDefaultScreen()
{
	if (s_screen == NULL)
	{
		s_screen.Set(new MonAPI::Screen(), true);
	}
	return s_screen;
}
#else
extern unsigned char* screen_buffer;
extern int screen_width, screen_height;
#ifdef WIN32
extern void MonaGUI_Invalidate();
#endif
#endif

static Rectangle GetScreenRectangle()
{
#ifdef MONA
	_P<MonAPI::Screen> scr = GetDefaultScreen();
	return Rectangle(0, 0, scr->getWidth(), scr->getHeight());
#else
	return Rectangle(0, 0, screen_width, screen_height);
#endif
}


void DrawImage(_P<Bitmap> dst, _P<Bitmap> src, int x, int y, int sx, int sy, int sw, int sh, bool srccopy)
{
	int dw = dst->get_Width(), dh = dst->get_Height();
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
	if (x2 > src->get_Width ()) x2 = src->get_Width ();
	if (y2 > src->get_Height()) y2 = src->get_Height();
	x1 -= sx;
	y1 -= sy;
	x2 -= sx;
	y2 -= sy;
	for (int yy = y1; yy < y2; yy++)
	{
		Color* pDst = &dst->get()[( x + x1) + ( y + yy) * dw];
		Color* pSrc = &src->get()[(sx + x1) + (sy + yy) * src->get_Width()];
		for (int xx = x1; xx < x2; xx++, pDst++, pSrc++)
		{
			if (srccopy || pSrc->get_A() != 0) *pDst = *pSrc;
		}
	}
}

namespace System { namespace Mona { namespace Forms
{
	extern _P<Form> activeForm;
	static _P<Font> s_font;
	
	_P<Font> Control::get_DefaultFont()
	{
		if (s_font == NULL) s_font = new Font(Application::defaultFontData);
		return s_font;
	}

	Control::Control()
	{
		this->bounds = Rectangle(0, 0, 32, 32);
		this->foreColor = DEFAULT_FORECOLOR;
		this->backColor = DEFAULT_BACKCOLOR;
		this->foreColorChanged = false;
		this->backColorChanged = false;
		this->visible = false;
		this->capture = false;
		this->transparencyKey = Color::get_Empty();
		this->_object = NULL;
		this->controls = new ControlCollection();
		this->controls->target = this;
	}
	
	Control::~Control()
	{
		this->Dispose();
	}
	
	void Control::Show()
	{
		if (this->visible) return;
		
		if (this->buffer == NULL) this->Create();
		if (this->parent == NULL) MonAPI::Message::sendReceive(NULL, __gui_server, MSG_GUISERVER_ACTIVATEWINDOW, this->get_Handle());
		activeForm = this;
		this->visible = true;
		this->_object->Visible = this->visible;
		this->Refresh();
	}
	
	void Control::Hide()
	{
		if (!this->visible) return;
		
		this->visible = false;
		if (this->parent != NULL) this->parent->Refresh();
	}
	
	void Control::Create()
	{
		if (this->buffer != NULL) return;
		
		this->buffer = new Bitmap(this->get_Width(), this->get_Height());
#ifdef MONA
		MessageInfo msg;
		if (MonAPI::Message::sendReceive(&msg, __gui_server, MSG_GUISERVER_CREATEWINDOW) != 0)
		{
			::printf("%s:%d:ERROR: can not connect to GUI server!\n", __FILE__, __LINE__);
			this->Dispose();
			return;
		}
		this->_object = (guiserver_window*)MonAPI::MemoryMap::map(msg.arg2);
		if (this->_object == NULL)
		{
			::printf("%s:%d:ERROR: can not create window!\n", __FILE__, __LINE__);
			this->Dispose();
			return;
		}
		if (this->parent != NULL) this->_object->Parent = this->parent->get_Handle();
		mapControls[this->get_Handle()] = this;
		this->_object->X = this->get_X();
		this->_object->Y = this->get_Y();
		this->_object->Width  = this->get_Width();
		this->_object->Height = this->get_Height();
		this->_object->OffsetX = this->offset.X;
		this->_object->OffsetY = this->offset.Y;
		this->_object->BufferHandle = this->buffer->get_Handle();
		this->_object->TransparencyKey = this->transparencyKey.ToArgb();
		this->_object->Protocol = 1;
#endif
		int len = this->get_Width() * this->get_Height();
		for (int i = 0; i < len; i++)
		{
			(*this->buffer.get())[i] = this->backColor;
		}
		if (this->parent != NULL)
		{
			if (!this->foreColorChanged) this->foreColor = this->parent->foreColor;
			if (!this->backColorChanged) this->backColor = this->parent->backColor;
		}
		
		FOREACH_AL(_P<Control>, ctrl, this->controls)
		{
			ctrl->Create();
		}
		END_FOREACH_AL
	}
	
	void Control::Close()
	{
		this->Dispose();
	}
	
	void Control::Dispose()
	{
		this->Hide();

		mapControls[this->get_Handle()] = NULL;
		MonAPI::Message::sendReceive(NULL, __gui_server, MSG_GUISERVER_DISPOSEWINDOW, this->get_Handle());
		this->_object = NULL;

		this->parent = NULL;
		this->buffer = NULL;
		
		this->TextChanged = NULL;
		this->ForeColorChanged = NULL;
		this->BackColorChanged = NULL;
		this->MouseMove = NULL;
		this->MouseDown = NULL;
		this->MouseUp = NULL;
		this->Click = NULL;

		FOREACH_AL(_P<Control>, ctrl, this->controls)
		{
			ctrl->Dispose();
		}
		END_FOREACH_AL
		this->controls->Clear();
		//this->controls->target = NULL;
	}
	
	void Control::Refresh()
	{
		if (this->buffer == NULL || !this->visible) return;
		
		this->RefreshInternal();
#ifdef MONA
		MonAPI::Message::sendReceive(NULL, __gui_server, MSG_GUISERVER_DRAWWINDOW, this->get_TopLevelControl()->get_Handle());
#endif
	}
	
	void Control::RefreshInternal()
	{
		if (this->buffer == NULL) return;
		
		Rectangle r(0, 0, this->get_Width(), this->get_Height());
		int x = 0, y = 0;
		_P<Control> form;
		for (_P<Control> c = this; c != NULL; c = c->parent)
		{
			if (!c->visible) return;
			
			if (c->parent != NULL)
			{
				r.X += c->get_X();
				r.Y += c->get_Y();
				r.Intersect(Rectangle(Point(), c->parent->get_ClientSize()));
				r.X += c->parent->offset.X;
				r.Y += c->parent->offset.Y;
				x += c->get_X() + c->parent->offset.X;
				y += c->get_Y() + c->parent->offset.Y;
			}
			form = c;
		}
		
		this->OnPaint();
		DrawImage(((Form*)form.get())->formBuffer, this->buffer, r.X, r.Y, r.X - x, r.Y - y, r.Width, r.Height, this->parent == NULL);
		FOREACH_AL(_P<Control>, ctrl, this->controls)
		{
			ctrl->RefreshInternal();
		}
		END_FOREACH_AL
	}
	
	_P<Graphics> Control::CreateGraphics()
	{
		_P<Graphics> ret = Graphics::FromImage(this->buffer);
		Size sz = this->get_ClientSize();
		ret->set_ClientRectangle(Rectangle(this->offset.X, this->offset.Y, sz.Width, sz.Height));
		return ret;
	}
	
	Point Control::PointToClient(Point p)
	{
		p -= this->get_Location() + this->offset;
		return this->parent == NULL ? p : this->parent->PointToClient(p);
	}
	
	Point Control::PointToScreen(Point p)
	{
		p += this->get_Location() + this->offset;
		return this->parent == NULL ? p : this->parent->PointToScreen(p);
	}
	
	_P<Control> Control::get_TopLevelControl()
	{
		return this->parent == NULL ? this : this->parent->get_TopLevelControl();
	}
	
	_P<Control> Control::FindControl(int x, int y)
	{
		if (!this->bounds.Contains(x, y)) return NULL;
		
		x -= this->get_X();
		y -= this->get_Y();
		if (this->buffer->GetPixel(x, y).get_A() == 0) return NULL;
		
		x -= this->offset.X;
		y -= this->offset.Y;
		Size sz = this->get_ClientSize();
		if (x >= sz.Width || y >= sz.Height) return this;
		
		FOREACH_AL(_P<Control>, c, this->controls)
		{
			_P<Control> fc = c->FindControl(x, y);
			if (fc != NULL) return fc;
		}
		END_FOREACH_AL
		return this;
	}
	
	void Control::set_Location(Point p)
	{
		if (this->get_Location() == p) return;
		
		this->bounds.X = p.X;
		this->bounds.Y = p.Y;
		if (this->_object == NULL) return;
		
		if (this->parent == NULL)
		{
			MonAPI::Message::sendReceive(NULL, __gui_server, MSG_GUISERVER_MOVEWINDOW, this->get_Handle(), (dword)p.X, (dword)p.Y);
		}
		else
		{
			this->_object->X = p.X;
			this->_object->Y = p.Y;
			this->parent->Refresh();
		}
	}
	
	Size Control::get_ClientSize()
	{
		int bw = this->offset.X;
		return Size(this->get_Width() - bw * 2, this->get_Height() - this->offset.Y - bw);
	}
	
	void Control::set_ClientSize(Size sz)
	{
		int bw = this->offset.X;
		this->set_Size(Size(sz.Width + bw * 2, sz.Height + this->offset.Y + bw));
	}
	
	void Control::set_Visible(bool v)
	{
		if (v) this->Show(); else this->Hide();
	}
	
	void Control::set_Capture(bool v)
	{
		if (v == this->capture) return;
		
		this->capture = v;
		MonAPI::Message::sendReceive(NULL, __gui_server, MSG_GUISERVER_MOUSECAPTURE, this->get_Handle(), v ? 1 : 0);
	}
	
	void Control::set_Text(String text)
	{
		if (this->text == text) return;
		
		this->text = text;
		this->OnTextChanged(EventArgs::get_Empty());
	}
	
	void Control::OnTextChanged(_P<EventArgs> e)
	{
		this->raise_TextChanged(this, e);
		if (this->buffer == NULL) return;
		
		this->Refresh();
	}
	
	void Control::set_ForeColor(Color c)
	{
		if (this->foreColor == c) return;
		
		this->foreColor = c;
		this->foreColorChanged = true;
		this->OnForeColorChanged(EventArgs::get_Empty());
	}
	
	void Control::OnForeColorChanged(_P<EventArgs> e)
	{
		this->raise_ForeColorChanged(this, e);
		if (this->buffer == NULL) return;
		
		this->Refresh();
	}
	
	void Control::set_BackColor(Color c)
	{
		if (this->backColor == c) return;
		
		this->backColor = c;
		this->backColorChanged = true;
		this->OnBackColorChanged(EventArgs::get_Empty());
	}
	
	void Control::OnBackColorChanged(_P<EventArgs> e)
	{
		this->raise_BackColorChanged(this, e);
		if (this->buffer == NULL) return;
		
		this->Refresh();
	}
	
	void Control::set_TransparencyKey(Color c)
	{
		this->transparencyKey = c;
		if (this->_object == NULL) return;
		
		unsigned int v = c.ToArgb();
		if (this->_object->TransparencyKey == v) return;
		
		this->_object->TransparencyKey = v;
		this->Refresh();
	}
	
	Control::NCState Control::NCHitTest(int x, int y)
	{
		return Rectangle(Point::get_Empty(), this->get_ClientSize()).Contains(x, y)
			? NCState_Client : NCState_None;
	}
	
	void Control::WndProc(dword type, _P<EventArgs> e)
	{
		switch (type)
		{
			case MSG_GUISERVER_MOUSEMOVE:
			{
				_P<MouseEventArgs> arg = (MouseEventArgs*)e.get();
				Point pt_arg(arg->X, arg->Y);
				Point pt = arg->Button == 0 ? pt_arg : this->clickPoint;
				if (this->NCHitTest(pt.X, pt.Y) == NCState_Client)
				{
					if (arg->Button != 0 || Rectangle(Point(), this->get_Size()).Contains(pt_arg)) this->OnMouseMove(arg);
				}
				else
				{
					this->OnNCMouseMove(arg);
				}
				break;
			}
			case MSG_GUISERVER_MOUSEDOWN:
			{
				_P<MouseEventArgs> arg = (MouseEventArgs*)e.get();
				this->clickPoint = Point(arg->X, arg->Y);
				this->NCHitTest(arg->X, arg->Y) == NCState_Client
					? this->OnMouseDown(arg) : this->OnNCMouseDown(arg);
				break;
			}
			case MSG_GUISERVER_MOUSEUP:
			{
				_P<MouseEventArgs> arg = (MouseEventArgs*)e.get();
				this->NCHitTest(this->clickPoint.X, this->clickPoint.Y) == NCState_Client
					? this->OnMouseUp(arg) : this->OnNCMouseUp(arg);
				break;
			}
			case MSG_GUISERVER_MOUSEENTER:
				this->OnMouseEnter(EventArgs::get_Empty());
				break;
			case MSG_GUISERVER_MOUSELEAVE:
				this->OnMouseLeave(EventArgs::get_Empty());
				break;
			case MSG_GUISERVER_KEYDOWN:
				this->OnKeyDown((KeyEventArgs*)e.get());
				break;
			case MSG_GUISERVER_KEYUP:
				this->OnKeyUp((KeyEventArgs*)e.get());
				break;
			default:
				break;
		}
	}
	
	void Control::OnMouseMove(_P<MouseEventArgs> e)
	{
		this->raise_MouseMove(this, e);
	}
	
	void Control::OnMouseDown(_P<MouseEventArgs> e)
	{
		this->raise_MouseDown(this, e);
	}
	
	void Control::OnMouseUp(_P<MouseEventArgs> e)
	{
		this->raise_MouseUp(this, e);
	}
	
	void Control::OnMouseEnter(_P<EventArgs> e)
	{
		this->raise_MouseEnter(this, e);
	}
	
	void Control::OnMouseLeave(_P<EventArgs> e)
	{
		this->raise_MouseLeave(this, e);
	}
	
	void Control::OnClick(_P<EventArgs> e)
	{
		this->raise_Click(this, e);
	}
	
	void Control::OnKeyDown(_P<KeyEventArgs> e)
	{
		this->raise_KeyDown(this, e);
	}
	
	void Control::OnKeyUp(_P<KeyEventArgs> e)
	{
		this->raise_KeyUp(this, e);
	}
	
	
	#define BASE ArrayList<_P<Control> >
	
	void Control::ControlCollection::Add(_P<Control> control)
	{
		BASE::Add(control);
		
		control->parent = this->target;
		control->visible = true;
	}
}}}
