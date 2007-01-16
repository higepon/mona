// This file is in the public domain.
// There are no restrictions on any sort of usage of this file.

#ifdef MONA
#include <monapi.h>
#include <monapi/messages.h>
#include <servers/gui.h>

extern uint32_t __gui_server;
#endif
#include <gui/System/Mona/Forms/Form.h>
#include <gui/System/Mona/Forms/Application.h>
#include <gui/System/Mona/Forms/ControlPaint.h>
#include <gui/System/Drawing/Font.h>
#include <gui/System/Math.h>

#define BASE Control

using namespace System;
using namespace System::Drawing;

static void EraseRectangle(_P<Bitmap> bmp, int x, int y, int w, int h)
{
	int x1 = x, y1 = y, x2 = x + w, y2 = y + h;
	int bw = bmp->get_Width(), bh = bmp->get_Height();
	if (x1 < 0) x1 = 0;
	if (y1 < 0) y1 = 0;
	if (x2 > bw) x2 = bw;
	if (y2 > bh) y2 = bh;
	Color empty = Color::get_Empty();
	for (int yy = y1; yy < y2; yy++)
	{
		Color* buf = &bmp->get()[x1 + yy * bw];
		for (int xx = x1; xx < x2; xx++, buf++)
		{
			*buf = empty;
		}
	}
}

namespace System { namespace Mona { namespace Forms
{
	_P<Form> activeForm;
	
	_P<Form> Form::get_ActiveForm()
	{
		return activeForm;
	}
	
	Form::Form() : isCloseButtonPushed(false), ncState(NCState_None), opacity(1.0)
	{
		this->offset = Point(2, Control::get_DefaultFont()->get_Height() + 8);
#ifdef MONA
		this->overlap = 0;
#endif
	}
	
	Form::~Form()
	{
		this->Dispose();
	}
	
	void Form::Hide()
	{
		if (!this->get_Visible()) return;
		
		BASE::Hide();
		this->Erase();
	}
	
	void Form::Create()
	{
		BASE::Create();
		Application::AddForm(this);
		this->isCloseButtonPushed = false;
		this->ncState = NCState_None;
		this->formBuffer = new Bitmap(this->get_Width(), this->get_Height());
		this->_object->FormBufferHandle = this->formBuffer->get_Handle();
		this->_object->Opacity = (int)(this->opacity * 255.0);
	}
	
	void Form::Dispose()
	{
		if (this->overlap != 0)
		{
			MonAPI::Message::sendReceive(NULL, __gui_server, MSG_GUISERVER_DISPOSEOVERLAP, this->overlap);
			this->overlap = 0;
		}
		this->ncState = NCState_None;
		BASE::Dispose();
		Application::RemoveForm(this);
		this->formBuffer = NULL;
	}
	
	void Form::Erase()
	{
		if (this->buffer == NULL) return;
		
#ifdef MONA
		this->_object->Visible = false;
		::monapi_call_mouse_set_cursor(MONAPI_FALSE);
		MonAPI::Message::sendReceive(NULL, __gui_server, MSG_GUISERVER_DRAWWINDOW, this->get_Handle());
		::monapi_call_mouse_set_cursor(MONAPI_TRUE);
		this->_object->Visible = this->get_Visible();
#else
		Size sz = this->get_Size();
		_P<Bitmap> bmp = new Bitmap(sz.Width, sz.Height);
		int len = sz.Width * sz.Height;
		Color white = Color::get_White();
		Color empty = Color::get_Empty();
		for (int i = 0; i < len; i++)
		{
			(*bmp.get())[i] = (*this->buffer.get())[i].get_A() != 0 ? white : empty;
		}
		this->DrawImage(bmp);
#endif
	}
	
	bool Form::CheckPoint(int x, int y)
	{
		return this->get_Bounds().Contains(x, y)
			&& this->buffer->GetPixel(x - this->get_X(), y - this->get_Y()).get_A() != 0;
	}
	
	void Form::set_Opacity(double op)
	{
		if (op < 0.0) op = 0.0;
		if (op > 1.0) op = 1.0;
		this->opacity = op;
		if (this->_object == NULL) return;
		
		int v = (int)(op * 255.0);
		if (this->_object->Opacity == v) return;
		
		this->_object->Opacity = v;
		this->Refresh();
	}
	
	void Form::WndProc(uint32_t type, _P<EventArgs> e)
	{
		switch (type)
		{
			case MSG_GUISERVER_ACTIVATED:
				if (activeForm != this)
				{
					activeForm = this;
					this->Refresh();
				}
				this->OnActivated(EventArgs::get_Empty());
				break;
			case MSG_GUISERVER_DEACTIVATE:
				activeForm = NULL;
				this->Refresh();
				this->OnDeactivate(EventArgs::get_Empty());
				break;
			default:
				BASE::WndProc(type, e);
				break;
		}
	}
	
	void Form::OnPaint()
	{
		if (this->buffer == NULL || (this->_object->Flags & WINDOWFLAGS_NOBORDER) != 0) return;
		
		_P<Graphics> g = Graphics::FromImage(this->buffer);
		int w = this->get_Width(), h = this->get_Height(), oy = this->offset.Y;
		_P<Font> f = Control::get_DefaultFont();
		Size sz = g->MeasureString(this->get_Text(), f);
		int tx = 2 + 2 + (oy - 8) + 2, tw = Math::Min(tx + sz.Width + 1 + 2 + 2, w);
		Color tf = Color::get_Black(), tb = activeForm == this ? Color::FromArgb(0xff, 0xe0, 0) : ControlPaint::get_Light();
		
		// Title Bar
		ControlPaint::DrawEngraved(g, 0, 0, tw, oy - 2);
		if (tw < w) EraseRectangle(this->buffer, tw, 0, w - tw, oy - 2);
		g->FillRectangle(tb, 2, 2, tw - 4, oy - 4);
		
		// Close Button
		ControlPaint::DrawEngraved(g, 4, 4, oy - 8, oy - 8);
		if (this->isCloseButtonPushed)
		{
			g->FillRectangle(Color::FromArgb(0x80, 0x80, 0), 6, 6, oy - 12, oy - 12);
		}
		
		// Border
		if (this->get_BackColor() != Color::get_Empty())
		{
			ControlPaint::DrawEngraved(g, 0, oy - 2, w, h - (oy - 2));
		}
		
		// Caption
		g->set_ClientRectangle(Rectangle(tx, 4, w - tx - 4, oy - 8));
		g->DrawString(this->get_Text(), f, tf, 0, 0);
		g->DrawString(this->get_Text(), f, tf, 1, 0);
		
		g->Dispose();
	}
	
	Control::NCState Form::NCHitTest(int x, int y)
	{
		if ((this->_object->Flags & WINDOWFLAGS_NOBORDER) != 0 || BASE::NCHitTest(x, y) == NCState_Client)
		{
			return NCState_Client;
		}
		
		int oy = this->offset.Y, xx = x + this->offset.X, yy = y + oy;
		if (Rectangle(4, 4, oy - 8, oy - 8).Contains(xx, yy)) return NCState_CloseButton;
		if (yy < oy) return NCState_TitleBar;
		return NCState_None;
	}
	
#ifndef MONA
	void Form::DrawReversibleRectangle()
	{
		Rectangle r = this->get_Bounds();
		r.X += this->ptRevRect.X - this->clickPoint.X;
		r.Y += this->ptRevRect.Y - this->clickPoint.Y;
		//r.X--; r.Y--; r.Width += 2; r.Height += 2;
		for (int i = 0; i < 1; i++)
		{
			ControlPaint::DrawReversibleRectangle(r);
			r.X++; r.Y++; r.Width -= 2; r.Height -= 2;
		}
	}
#endif
	
	void Form::OnNCMouseMove(_P<MouseEventArgs> e)
	{
		switch (this->ncState)
		{
			case NCState_CloseButton:
			{
				bool pushed = this->NCHitTest(e->X, e->Y) == NCState_CloseButton;
				if (this->isCloseButtonPushed != pushed)
				{
					this->isCloseButtonPushed = pushed;
					this->Refresh();
				}
				break;
			}
			case NCState_TitleBar:
			{
				if (e->Button == 0)
				{
					if (this->get_Capture()) this->set_Capture(false);
#ifdef MONA
					if (this->overlap != 0)
					{
						MonAPI::Message::sendReceive(NULL, __gui_server, MSG_GUISERVER_DISPOSEOVERLAP, this->overlap);
						this->overlap = 0;
					}
#endif
					this->ncState = NCState_None;
				}
				else
				{
#ifdef MONA
					int ex = this->get_X() + (e->X - this->clickPoint.X), ey = this->get_Y() + (e->Y - this->clickPoint.Y);
					if (this->ptRevRect.X != ex || this->ptRevRect.Y != ey)
					{
						this->ptRevRect = Point(ex, ey);
						MonAPI::Message::sendReceive(NULL, __gui_server, MSG_GUISERVER_MOVEOVERLAP, this->overlap,
							MAKE_DWORD(ex, ey), MAKE_DWORD(this->get_Width(), this->get_Height()));
					}
#else
					this->DrawReversibleRectangle();
					this->ptRevRect = Point(e->X, e->Y);
					this->DrawReversibleRectangle();
#endif
				}
				break;
			}
			default:
				break;
		}
		
		BASE::OnNCMouseMove(e);
	}
	
	void Form::OnNCMouseDown(_P<MouseEventArgs> e)
	{
		if (this->ncState == NCState_TitleBar)
		{
			if (this->get_Capture()) this->set_Capture(false);
#ifdef MONA
			if (this->overlap != 0)
			{
				MonAPI::Message::sendReceive(NULL, __gui_server, MSG_GUISERVER_DISPOSEOVERLAP, this->overlap);
				this->overlap = 0;
			}
#endif
			this->ncState = NCState_None;
		}
		if (e->Button == 1)
		{
			switch (this->ncState = this->NCHitTest(e->X, e->Y))
			{
				case NCState_CloseButton:
					this->set_Capture(true);
					this->isCloseButtonPushed = true;
					this->Refresh();
					break;
				case NCState_TitleBar:
				{
					this->set_Capture(true);
					this->ptRevRect = Point(e->X, e->Y);
#ifdef MONA
					MessageInfo msg;
					MonAPI::Message::sendReceive(&msg, __gui_server, MSG_GUISERVER_CREATEOVERLAP,
						this->get_X(), this->get_Y(),
						MAKE_DWORD(this->get_Width(), this->get_Height()));
					this->overlap = msg.arg2;
#else
					this->DrawReversibleRectangle();
#endif
					break;
				}
				default:
					break;
			}
		}
		
		BASE::OnNCMouseDown(e);
	}
	
	void Form::OnNCMouseUp(_P<MouseEventArgs> e)
	{
		bool destroy = false;
		
		if (e->Button == 1)
		{
			destroy = this->NCHitTest(e->X, e->Y) == NCState_CloseButton && this->ncState == NCState_CloseButton;
			
			switch (this->ncState)
			{
				case NCState_CloseButton:
				{
					this->set_Capture(false);
					this->isCloseButtonPushed = false;
					break;
				}
				case NCState_TitleBar:
				{
#ifdef MONA
					MonAPI::Message::sendReceive(NULL, __gui_server, MSG_GUISERVER_DISPOSEOVERLAP, this->overlap);
					this->overlap = 0;
#else
					this->DrawReversibleRectangle();
#endif
					this->set_Capture(false);
					Point p = this->get_Location();
					p.X += e->X - this->clickPoint.X;
					p.Y += e->Y - this->clickPoint.Y;
					this->set_Location(p);
					break;
				}
				default:
					this->Refresh();
					break;
			}
		}
		
		BASE::OnNCMouseUp(e);
		
		this->ncState = NCState_None;
		if (destroy) this->Dispose();
	}
	
	void Form::OnActivated(_P<EventArgs> e)
	{
		this->raise_Activated(this, e);
	}
	
	void Form::OnDeactivate(_P<EventArgs> e)
	{
		this->raise_Deactivate(this, e);
	}
}}}
