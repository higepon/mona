// This file is in the public domain.
// There are no restrictions on any sort of usage of this file.

#ifdef MONA
#include <monapi.h>
#endif

#include <gui/System/Mona/Forms/Application.h>
#include <gui/System/Mona/Forms/Control.h>
#include <gui/System/Mona/Forms/ControlPaint.h>

using namespace System;
using namespace System::Collections;
using namespace System::Drawing;

#define DEFAULT_FORECOLOR Color::get_Black()
#define DEFAULT_BACKCOLOR ControlPaint::get_Light()

#ifdef MONA
_P<MonAPI::Screen> GetDefaultScreen()
{
	static _P<MonAPI::Screen> ret;
	if (ret == NULL)
	{
		ret.Set(new MonAPI::Screen(), true);
	}
	return ret;
}

extern dword __mouse_server;

void __SetMouseCursor(bool enabled)
{
	dword hdr = enabled ? MSG_MOUSE_ENABLE_CURSOR : MSG_MOUSE_DISABLE_CURSOR;
	MonAPI::Message::sendReceive(NULL, __mouse_server, hdr);
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

namespace System { namespace Mona { namespace Forms
{
	_P<Font> Control::get_DefaultFont()
	{
		static _P<Font> ret;
		if (ret == NULL) ret = new Font(Application::defaultFontData);
		return ret;
	}

	Control::Control()
	{
		this->bounds = Rectangle(0, 0, 32, 32);
		this->foreColor = DEFAULT_FORECOLOR;
		this->backColor = DEFAULT_BACKCOLOR;
		this->foreColorChanged = false;
		this->backColorChanged = false;
		this->visible = false;
		this->controls = new ControlCollection(this);
	}
	
	Control::~Control()
	{
		this->Dispose();
	}
	
	void Control::Show()
	{
		if (this->visible) return;
		
		if (this->buffer == NULL) this->Create();
		this->visible = true;
		this->Refresh();
	}
	
	void Control::Hide()
	{
		if (!this->visible) return;
		
		this->visible = false;
		this->parent->Refresh();
	}
	
	void Control::Create()
	{
		if (this->buffer != NULL) return;
		
		this->buffer = new Bitmap(this->get_Width(), this->get_Height());
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
		this->OnPaint();
		
		FOREACH_AL(_P<Control>, ctrl, this->controls)
		{
			ctrl->Create();
		}
	}
	
	void Control::Close()
	{
		this->Dispose();
	}
	
	void Control::Dispose()
	{
		this->Hide();
		this->buffer = NULL;
		
		this->MouseMove = NULL;
		this->MouseDown = NULL;
		this->MouseUp = NULL;
		this->Click = NULL;
		
		FOREACH_AL(_P<Control>, ctrl, this->controls)
		{
			ctrl->Dispose();
		}
		this->controls->Clear();
	}
	
	void Control::Refresh()
	{
#ifdef MONA
		::__SetMouseCursor(false);
#endif
		this->RefreshInternal();
#ifdef MONA
		::__SetMouseCursor(true);
#endif
	}
	
	void Control::RefreshInternal()
	{
		for (_P<Control> c = this; c != NULL; c = c->parent)
		{
			if (!c->visible) return;
		}
		
		this->DrawImage(this->buffer);
		FOREACH_AL(_P<Control>, ctrl, this->controls)
		{
			ctrl->RefreshInternal();
		}
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
		return this;
	}
	
	Rectangle Control::get_VisibleRectangle()
	{
		Rectangle par = this->parent == NULL ? ::GetScreenRectangle() : this->parent->get_VisibleRectangle();
		par.Offset(-(this->get_X() + this->offset.X), -(this->get_Y() + this->offset.Y));
		Rectangle ret(Point::get_Empty(), this->get_ClientSize());
		ret.Intersect(par);
		return ret;
	}
	
	void Control::DrawImage(_P<Bitmap> image)
	{
#ifdef MONA
		_P<MonAPI::Screen> scr = ::GetDefaultScreen();
		unsigned char* vram = scr->getVRAM();
		int bpp = scr->getBpp(), sw = scr->getWidth(), sh = scr->getHeight();
#else
		unsigned char* vram = screen_buffer;
		int bpp = 32, sw = screen_width, sh = screen_height;
#endif
		int bypp = bpp >> 3;
		Point sp = this->PointToScreen(Point::get_Empty());
		sp -= this->offset;
		Rectangle r;
		if (this->parent == NULL)
		{
			r = Rectangle(-this->get_X(), -this->get_Y(), sw, sh);
		}
		else
		{
			r = this->parent->get_VisibleRectangle();
			r.Offset(-this->get_X(), -this->get_Y());
			Rectangle rr = Rectangle(0, 0, this->get_Width(), this->get_Height());
			r.Intersect(rr);
		}
		int rr = r.get_Right(), rb = r.get_Bottom();
		Color* pBuf = image->get();
		for (int y = 0; y < this->get_Height(); y++)
		{
			if (y < r.Y || rb <= y)
			{
				pBuf += this->get_Width();
				continue;
			}
			
			int sy = sp.Y + y;
			unsigned char* pVram = &vram[(sp.X + sy * sw) * bypp];
			for (int x = 0; x < this->get_Width(); x++, pVram += bypp, pBuf++)
			{
				if (x < r.X || rr <= x || pBuf->get_A() == 0) continue;
				
				switch (bpp)
				{
					case 8: // broken
					{
						*pVram = (pBuf->get_R() + pBuf->get_G() + pBuf->get_B()) / 3;
						break;
					}
					case 16: // broken
						*(unsigned short*)pVram = (unsigned short)pBuf->ToArgb();
						break;
					case 24:
						if (pVram[0] != pBuf->get_B()) pVram[0] = pBuf->get_B();
						if (pVram[1] != pBuf->get_G()) pVram[1] = pBuf->get_G();
						if (pVram[2] != pBuf->get_R()) pVram[2] = pBuf->get_R();
						break;
					case 32:
					{
						unsigned int c = pBuf->ToArgb() & 0xffffff;
						if (*(unsigned int*)pVram != c) *(unsigned int*)pVram = c;
						break;
					}
				}
			}
		}
#if !defined(MONA) && defined(WIN32)
		MonaGUI_Invalidate();
#endif
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
		
		this->OnPaint();
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
		
		this->OnPaint();
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
		
		this->OnPaint();
		this->Refresh();
	}
	
	void Control::WndProc(MessageType type, _P<EventArgs> e)
	{
		switch (type)
		{
			case WM_MOUSEMOVE:
			{
				_P<MouseEventArgs> arg = (MouseEventArgs*)e.get();
				Point pt = arg->Button == 0 ? Point(arg->X, arg->Y) : this->clickPoint;
				(Rectangle(Point::get_Empty(), this->get_ClientSize()).Contains(pt))
					? this->OnMouseMove(arg) : this->OnNCMouseMove(arg);
				break;
			}
			case WM_MOUSEDOWN:
			{
				_P<MouseEventArgs> arg = (MouseEventArgs*)e.get();
				this->clickPoint = Point(arg->X, arg->Y);
				(Rectangle(Point::get_Empty(), this->get_ClientSize()).Contains(arg->X, arg->Y))
					? this->OnMouseDown(arg) : this->OnNCMouseDown(arg);
				break;
			}
			case WM_MOUSEUP:
			{
				_P<MouseEventArgs> arg = (MouseEventArgs*)e.get();
				(Rectangle(Point::get_Empty(), this->get_ClientSize()).Contains(this->clickPoint))
					? this->OnMouseUp(arg) : this->OnNCMouseUp(arg);
				break;
			}
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
	
	void Control::OnClick(_P<EventArgs> e)
	{
		this->raise_Click(this, e);
	}
	
	
	#define BASE ArrayList<_P<Control> >
	
	void Control::ControlCollection::Add(_P<Control> control)
	{
		BASE::Add(control);
		
		control->parent = this->parent;
		control->visible = true;
	}
}}}
