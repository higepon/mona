// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#include <gui/System/Mona/Forms/Button.h>
#include <gui/System/Drawing/Font.h>
#include <gui/System/Math.h>
#include <gui/System/Mona/Forms/ControlPaint.h>

#define BASE Control

using namespace System;
using namespace System::Drawing;

namespace System { namespace Mona { namespace Forms
{
	Button::Button() : isPushed(false)
	{
	}
	
	Button::~Button()
	{
	}
	
	void Button::Create()
	{
		BASE::Create();
		this->isPushed = false;
	}
	
	void Button::OnPaint()
	{
		_P<Graphics> g = this->CreateGraphics();
		int w = this->get_Width(), h = this->get_Height();
		
		// Erase
		g->FillRectangle(this->get_BackColor(), 0, 0, w, h);
		
		// Text
		_P<Font> f = Control::get_DefaultFont();
		Size sz = g->MeasureString(this->get_Text(), f);
		int x = (w - sz.Width) / 2, y = (h - sz.Height) / 2;
		if (this->isPushed)
		{
			x++;
			y++;
		}
		g->DrawString(this->get_Text(), f, this->get_ForeColor(), x, y);
		
		// Border
		if (this->isPushed)
		{
			ControlPaint::DrawSunken(g, 0, 0, w, h);
		}
		else
		{
			ControlPaint::DrawRaised(g, 0, 0, w, h);
		}
		
		g->Dispose();
	}
	
	void Button::OnMouseMove(_P<MouseEventArgs> e)
	{
		if (e->Button != 0)
		{
			bool pushed = Rectangle(Point::get_Empty(), this->get_ClientSize()).Contains(e->X, e->Y);
			if (this->isPushed != pushed)
			{
				this->isPushed = pushed;
				this->OnPaint();
				this->Refresh();
			}
		}
		
		BASE::OnMouseMove(e);
	}
	
	void Button::OnMouseDown(_P<MouseEventArgs> e)
	{
		this->isPushed = true;
		this->OnPaint();
		this->Refresh();
		
		BASE::OnMouseDown(e);
	}
	
	void Button::OnMouseUp(_P<MouseEventArgs> e)
	{
		if (this->isPushed)
		{
			if (e->Button == 1) this->OnClick(EventArgs::get_Empty());
			
			this->isPushed = false;
			this->OnPaint();
			this->Refresh();
		}
		
		BASE::OnMouseUp(e);
	}
}}}
