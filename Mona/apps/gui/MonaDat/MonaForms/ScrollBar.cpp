// This file is in the public domain.
// There are no restrictions on any sort of usage of this file.

#include <gui/System/Math.h>
#include <gui/System/Mona/Forms/ControlPaint.h>
#ifdef MONA
#include <gui/System/Mona/Forms/Cursor.h>
#endif
#include "HScrollBar.h"
#include "VScrollBar.h"

using namespace System;
using namespace System::Drawing;

namespace System { namespace Mona { namespace Forms
{
	/*internal*/ class ScrollBarButton : public Object
	{
	public:
		int Type;
		bool IsPushed;
		Rectangle Bounds;
		
		ScrollBarButton() : Type(0), IsPushed(false) {}
		virtual ~ScrollBarButton() {}
		
		void Draw(_P<Graphics> g, Color back)
		{
			int w = this->Bounds.Width, h = this->Bounds.Height;
			
			// Erase
			g->FillRectangle(back, 0, 0, w, h);
			
			// Border
			if (this->IsPushed)
			{
				ControlPaint::DrawSunken(g, 0, 0, w, h);
			}
			else
			{
				ControlPaint::DrawRaised(g, 0, 0, w, h);
			}
			
			int sw = w - 2, sh = h - 2;
			_A<Point> p(3);
			p[0] = p[1] = p[2] = Point(sw / 2 - 2, sh / 2 - 2);
			if (this->Type == 0)
			{
				p[0].X = p[1].X - 7;
				p[2].X = p[1].X + 7;
			}
			else
			{
				p[0].Y = p[1].Y - 7;
				p[2].Y = p[1].Y + 7;
			}
			Color ll = ControlPaint::get_LightLight();
			Color d  = ControlPaint::get_Dark();
			int offset = this->IsPushed ? 1 : 0;
			FOREACH (Point, pp, p)
			{
				if ((this->Type == 0 && (pp.X < 4 || pp.X >= sw - 8))
					|| (this->Type != 0 && (pp.Y < 4 || pp.Y >= sh - 8))) continue;
				
				int x = pp.X + offset, y = pp.Y + offset;
				g->DrawLine(ll, x, y, x + 4, y);
				g->DrawLine(ll, x, y, x, y + 4);
				g->DrawLine(d, x + 4, y, x + 4, y + 4);
				g->DrawLine(d, x, y + 4, x + 4, y + 4);
			} END_FOREACH
			g->Dispose();
		}
	};

	/*internal*/ class ArrowButton : public Control
	{
	private:
		int type, state;
		_P<Timer> timer1;

	public:
		ArrowButton() : type(0), state(0)
		{
			this->timer1 = new Timer();
			this->timer1->add_Tick(new EventHandler<ArrowButton>(this, &ArrowButton::timer1_Tick));
		}
		
		virtual ~ArrowButton() {}
		inline void set_Type(int type) { this->type = type; }
		
		virtual void Create()
		{
			Control::Create();
			this->state = 0;
		}
		
	    virtual void Dispose()
	    {
	        this->timer1->Dispose();
	        Control::Dispose();
	    }
		
	protected:
		void timer1_Tick(_P<Object> sender, _P<EventArgs> e)
		{
			this->timer1->Stop();
			if (this->timer1->get_Interval() == 10) return;
			
			this->OnClick(EventArgs::get_Empty());
			this->timer1->set_Interval(50);
			this->timer1->Start();
		}
		
		void DrawButton(_P<Graphics> g, Rectangle rect, int type, bool isPushed)
		{
			// Erase
			g->FillRectangle(this->get_BackColor(), rect.X, rect.Y, rect.Width, rect.Height);
			
			// Border
			if (isPushed)
			{
				ControlPaint::DrawSunken(g, rect.X, rect.Y, rect.Width, rect.Height);
			}
			else
			{
				ControlPaint::DrawRaised(g, rect.X, rect.Y, rect.Width, rect.Height);
			}
			
			int sw = rect.Width - 2, sh = rect.Height - 2;
			int x1 = 0, y1 = 0, x2 = 0, y2 = 0, x3 = 0, y3 = 0;
			switch (type)
			{
				case 0:
					x1 = 2; y1 = sh / 2; x2 = sw - 2; y2 = 2; x3 = x2; y3 = sh - 2;
					break;
				case 1:
					x1 = sw - 2; y1 = sh / 2; x2 = 2; y2 = 2; x3 = x2; y3 = sh - 2;
					break;
				case 2:
					x1 = sw / 2; y1 = 2; x2 = 2; y2 = sh - 2; x3 = sw - 2; y3 = y2;
					break;
				case 3:
					x1 = sw / 2; y1 = sh - 2; x2 = 2; y2 = 2; x3 = sw - 2; y3 = y2;
					break;
			}
			int add = isPushed ? 1 : 0;
			x1 += rect.X + add; y1 += rect.Y + add;
			x2 += rect.X + add; y2 += rect.Y + add;
			x3 += rect.X + add; y3 += rect.Y + add;
			Color ll = ControlPaint::get_LightLight();
			Color d  = ControlPaint::get_Dark();
			g->DrawLine(ll, x1 + 1, y1 + 1, x2 + 1, y2 + 1);
			g->DrawLine(ll, x2 + 1, y2 + 1, x3 + 1, y3 + 1);
			g->DrawLine(ll, x3 + 1, y3 + 1, x1 + 1, y1 + 1);
			g->DrawLine(d, x1, y1, x2, y2);
			g->DrawLine(d, x2, y2, x3, y3);
			g->DrawLine(d, x3, y3, x1, y1);
		}
		
		virtual void OnPaint()
		{
			_P<Graphics> g = this->CreateGraphics();
			int w = this->get_Width(), h = this->get_Height(), hw = w / 2, hh = h / 2;
			if (this->type == 0)
			{
				this->DrawButton(g, Rectangle(0, 0, hw, h), this->type, this->state == 1);
				this->DrawButton(g, Rectangle(hw, 0, w - hw, h), this->type + 1, this->state == 2);
			}
			else
			{
				this->DrawButton(g, Rectangle(0, 0, w, hh), this->type, this->state == 1);
				this->DrawButton(g, Rectangle(0, hh, w, h - hh), this->type + 1, this->state == 2);
			}
			g->Dispose();
		}
		
		void SetState(int x, int y)
		{
			int w = this->get_Width(), h = this->get_Height(), hw = w / 2, hh = h / 2;
			bool p1 = false, p2 = false;
			if (this->type == 0)
			{
				p1 = Rectangle(0, 0, hw, h).Contains(x, y);
				p2 = Rectangle(hw, 0, w - hw, h).Contains(x, y);
			}
			else
			{
				p1 = Rectangle(0, 0, w, hh).Contains(x, y);
				p2 = Rectangle(0, hh, w, h - hh).Contains(x, y);
			}
			this->state = p1 ? 1 : p2 ? 2 : 0;
		}
		
		virtual void OnMouseDown(_P<MouseEventArgs> e)
		{
			if (e->Button == 1)
			{
				this->set_Capture(true);
				this->SetState(e->X, e->Y);
				this->Refresh();
				this->timer1->set_Interval(800);
				this->timer1->Start();
				this->OnClick(EventArgs::get_Empty());
			}
			
			Control::OnMouseDown(e);
		}
		
		virtual void OnMouseMove(_P<MouseEventArgs> e)
		{
			if (e->Button == 1)
			{
				int st = this->state;
				this->SetState(e->X, e->Y);
				if (st != this->state)
				{
					this->Refresh();
					this->OnClick(EventArgs::get_Empty());
				}
			}
			
			Control::OnMouseMove(e);
		}
		
		virtual void OnMouseUp(_P<MouseEventArgs> e)
		{
			this->timer1->Stop();
			this->timer1->set_Interval(10);
			this->set_Capture(false);
			if (this->state != 0)
			{
				this->state = 0;
				this->Refresh();
			}
			
			Control::OnMouseUp(e);
		}
		
		virtual void OnClick(_P<EventArgs> e)
		{
			Control::OnClick(e);
			
			switch (this->state)
			{
				case 1:
					OnDecrease(e);
					break;
				case 2:
					OnIncrease(e);
					break;
			}
		}
		
		virtual void OnDecrease(_P<EventArgs> e)
		{
			this->raise_Decrease(this, e);
		}
		
		virtual void OnIncrease(_P<EventArgs> e)
		{
			this->raise_Increase(this, e);
		}
		
		DECLARE_EVENT(System::IEventHandler, Decrease, _P<EventArgs>)
		DECLARE_EVENT(System::IEventHandler, Increase, _P<EventArgs>)
	};
	
	
	// class ScrollBar
	
	ScrollBar::ScrollBar()
		: state(0), value(0), isBarTarget(false), Minimum(0), Maximum(0), SmallChange(1), LargeChange(8)
	{
		this->bar = new ScrollBarButton();
		
		_P<ArrowButton> bt1 = new ArrowButton();
		_P<ArrowButton> bt2 = new ArrowButton();
		this->bt1 = bt1.get();
		this->bt2 = bt2.get();
		bt1->add_Decrease(new EventHandler<ScrollBar>(this, &ScrollBar::dec_Click));
		bt1->add_Increase(new EventHandler<ScrollBar>(this, &ScrollBar::inc_Click));
		bt2->add_Decrease(new EventHandler<ScrollBar>(this, &ScrollBar::dec_Click));
		bt2->add_Increase(new EventHandler<ScrollBar>(this, &ScrollBar::inc_Click));
		this->timer1 = new Timer();
		this->timer1->add_Tick(new EventHandler<ScrollBar>(this, &ScrollBar::timer1_Tick));
	}
	
	ScrollBar::~ScrollBar()
	{
	}
	
	void ScrollBar::set_Value(int value)
	{
		if (value < this->Minimum) value = this->Minimum;
		if (value > this->Maximum) value = this->Maximum;
		if (value == this->value) return;
		
		this->value = value;
		this->SetButton();
		this->Refresh();
		this->OnValueChanged(EventArgs::get_Empty());
	}
	
	void ScrollBar::Create()
	{
		this->get_Controls()->Add(this->bt1.get());
		this->get_Controls()->Add(this->bt2.get());
		this->SetButton();
		
		Control::Create();
	}
	
    void ScrollBar::Dispose()
    {
        this->timer1->Dispose();
        Control::Dispose();
    }
	
	void ScrollBar::Calculate(int width)
	{
		if (this->Maximum < this->Minimum) this->Maximum = this->Minimum;
		if (this->value < this->Minimum) this->value = this->Minimum;
		if (this->value > this->Maximum) this->value = this->Maximum;
		
		this->barRange = this->barWidth = width;
		this->barPos = 0;
		int range = this->Maximum - this->Minimum;
		if (range <= 0 || this->LargeChange <= 0) return;
		
		this->barWidth = Math::Max(DefaultSize, this->LargeChange * width / (range + this->LargeChange));
		this->barPos = (this->value - this->Minimum) * (width - this->barWidth) / range;
	}
	
	void ScrollBar::OnPaint()
	{
		_P<Graphics> g = Graphics::FromImage(this->buffer);
		ControlPaint::DrawSunken(g, 0, 0, this->get_Width(), this->get_Height());
		_P<ScrollBarButton> b = (ScrollBarButton*)this->bar.get();
		g->set_ClientRectangle(b->Bounds);
		((ScrollBarButton*)this->bar.get())->Draw(g, this->get_BackColor());
		g->Dispose();
		
		Control::OnPaint();
	}
	
	void ScrollBar::OnMouseDown(_P<MouseEventArgs> e)
	{
		Control::OnMouseDown(e);
		if (e->Button != 1) return;
		
		this->set_Capture(true);
		_P<ScrollBarButton> b = (ScrollBarButton*)this->bar.get();
		if (b->Bounds.Contains(e->X, e->Y))
		{
			this->bar_MouseDown(e->X - b->Bounds.X, e->Y - b->Bounds.Y);
			this->isBarTarget = true;
			b->IsPushed = true;
			this->Refresh();
		}
		else
		{
			this->timer1->set_Interval(800);
			this->timer1->Start();
			this->SetState(e->X, e->Y);
			this->Refresh();
			this->OnClick(EventArgs::get_Empty());
		}
	}
	
	void ScrollBar::OnMouseMove(_P<MouseEventArgs> e)
	{
		Control::OnMouseMove(e);
		
		this->mouse.X = e->X;
		this->mouse.Y = e->Y;
		if (e->Button != 1) return;
		
		if (this->isBarTarget)
		{
			_P<ScrollBarButton> b = (ScrollBarButton*)this->bar.get();
			this->bar_MouseMove(e->X - b->Bounds.X, e->Y - b->Bounds.Y);
		}
		else
		{
			int st = this->state;
			this->SetState(e->X, e->Y);
			if (st != this->state)
			{
				this->Refresh();
				this->OnClick(EventArgs::get_Empty());
			}
		}
	}
	
	void ScrollBar::OnMouseUp(_P<MouseEventArgs> e)
	{
		Control::OnMouseUp(e);
		if (e->Button != 1) return;
		
		this->set_Capture(false);
		if (this->isBarTarget)
		{
			_P<ScrollBarButton> b = (ScrollBarButton*)this->bar.get();
			b->IsPushed = false;
			this->isBarTarget = false;
			this->Refresh();
		}
		else
		{
			this->timer1->Stop();
			this->timer1->set_Interval(10);
			if (this->state != 0)
			{
				this->state = 0;
				this->Refresh();
			}
		}
	}
	
	void ScrollBar::OnClick(_P<EventArgs> e)
	{
		switch (this->state)
		{
			case 1:
				this->value -= this->LargeChange;
				if (this->value < this->Minimum) this->value = this->Minimum;
				break;
			case 2:
				this->value += this->LargeChange;
				if (this->value > this->Maximum) this->value = this->Maximum;
				break;
		}
		
		this->SetButton();
#ifdef MONA
		Point p = this->PointToClient(Cursor::get_Position());
		this->SetState(p.X, p.Y);
#else
		this->SetState(this->mouse.X, this->mouse.Y);
#endif
		this->Refresh();
		this->OnValueChanged(EventArgs::get_Empty());
	}
	
	void ScrollBar::OnValueChanged(_P<EventArgs> e)
	{
		this->raise_ValueChanged(this, e);
	}
	
	void ScrollBar::SetButtonInternal(int x, int y, int w, int h)
	{
		_P<ScrollBarButton> b = (ScrollBarButton*)this->bar.get();
		b->Bounds = Rectangle(x, y, w, h);
		this->Refresh();
	}
	
	void ScrollBar::dec_Click(_P<Object> sender, _P<EventArgs> e)
	{
		this->set_Value(this->value - this->SmallChange);
	}
	
	void ScrollBar::inc_Click(_P<Object> sender, _P<EventArgs> e)
	{
		this->set_Value(this->value + this->SmallChange);
	}
	
	void ScrollBar::timer1_Tick(_P<Object> sender, _P<EventArgs> e)
	{
		this->timer1->Stop();
		if (this->timer1->get_Interval() == 10) return;
		
		this->OnClick(EventArgs::get_Empty());
		this->timer1->set_Interval(50);
		this->timer1->Start();
	}
	
	
	// class HScrollBar
	
	HScrollBar::HScrollBar() {}
	HScrollBar::~HScrollBar() {}
	
	void HScrollBar::Create()
	{
		Size sz = this->get_ClientSize();
		this->bt1->set_Bounds(Rectangle(0, 0, DefaultSize * 2, sz.Height));
		this->bt2->set_Bounds(Rectangle(sz.Width - DefaultSize * 2, 0, DefaultSize * 2, sz.Height));
		
		ScrollBar::Create();
	}
	
	void HScrollBar::SetButton()
	{
		Size sz = this->get_ClientSize();
		this->Calculate(sz.Width - DefaultSize * 4);
		this->SetButtonInternal(DefaultSize * 2 + this->barPos, 0, this->barWidth, DefaultSize);
	}
	
	void HScrollBar::SetState(int x, int y)
	{
		this->state = 0;
		
		int h = this->get_Height();
		if (y < 0 || y >= h) return;
		
		_P<ScrollBarButton> b = (ScrollBarButton*)this->bar.get();
		if (this->bt1->get_Width() <= x && x < b->Bounds.X)
		{
			this->state = 1;
		}
		else if (b->Bounds.X + b->Bounds.Width <= x && x < this->bt2->get_X())
		{
			this->state = 2;
		}
	}
	
	void HScrollBar::OnPaint()
	{
		_P<Graphics> g = this->CreateGraphics();
		_P<ScrollBarButton> b = (ScrollBarButton*)this->bar.get();
		int x1 = this->bt1->get_Width();
		int x2 = b->Bounds.X;
		int x3 = x2 + b->Bounds.Width;
		int x4 = this->bt2->get_X();
		int h = this->get_Height();
		g->FillRectangle(this->get_BackColor(), 0, 0, this->get_Width(), h);
		switch (this->state)
		{
			case 1:
				g->FillRectangle(ControlPaint::get_Dark(), x1, 0, x2 - x1, h);
				break;
			case 2:
				g->FillRectangle(ControlPaint::get_Dark(), x3, 0, x4 - x3, h);
				break;
		}
		g->Dispose();
		
		ScrollBar::OnPaint();
	}
	
	void HScrollBar::bar_MouseDown(int x, int y)
	{
		_P<ScrollBarButton> b = (ScrollBarButton*)this->bar.get();
		this->oldValue = this->value;
		this->pos1 = b->Bounds.X;
		this->pos2 = this->pos1 + x;
	}
	
	void HScrollBar::bar_MouseMove(int x, int y)
	{
		int w = this->barRange - this->barWidth;
		_P<ScrollBarButton> b = (ScrollBarButton*)this->bar.get();
		if (w == 0 || !b->IsPushed) return;
		
		int mx = b->Bounds.X + x;
		int bx = this->pos1 - DefaultSize * 2;
		if (-80 <= y && y < b->Bounds.Height + 80)
		{
			bx += mx - this->pos2;
		}
		int d = this->Maximum - this->Minimum;
		this->set_Value(this->Minimum + d * bx / w);
	}
	
	
	// class VScrollBar
	
	VScrollBar::VScrollBar() {}
	VScrollBar::~VScrollBar() {}
	
	void VScrollBar::Create()
	{
		Size sz = this->get_ClientSize();
		this->bt1->set_Bounds(Rectangle(0, 0, sz.Width, DefaultSize * 2));
		this->bt2->set_Bounds(Rectangle(0, sz.Height - DefaultSize * 2, sz.Width, DefaultSize * 2));
		((ArrowButton*)this->bt1.get())->set_Type(2);
		((ArrowButton*)this->bt2.get())->set_Type(2);
		
		ScrollBar::Create();
	}
	
	void VScrollBar::SetButton()
	{
		Size sz = this->get_ClientSize();
		this->Calculate(sz.Height - DefaultSize * 4);
		this->SetButtonInternal(0, DefaultSize * 2 + this->barPos, DefaultSize, this->barWidth);
		((ScrollBarButton*)this->bar.get())->Type = 1;
	}
	
	void VScrollBar::SetState(int x, int y)
	{
		this->state = 0;
		
		int w = this->get_Width();
		if (x < 0 || x >= w) return;
		
		_P<ScrollBarButton> b = (ScrollBarButton*)this->bar.get();
		if (this->bt1->get_Height() <= y && y < b->Bounds.Y)
		{
			this->state = 1;
		}
		else if (b->Bounds.Y + b->Bounds.Height <= y && y < this->bt2->get_Y())
		{
			this->state = 2;
		}
	}
	
	void VScrollBar::OnPaint()
	{
		_P<Graphics> g = this->CreateGraphics();
		_P<ScrollBarButton> b = (ScrollBarButton*)this->bar.get();
		int y1 = this->bt1->get_Height();
		int y2 = b->Bounds.Y;
		int y3 = y2 + b->Bounds.Height;
		int y4 = this->bt2->get_Y();
		int w = this->get_Width();
		g->FillRectangle(this->get_BackColor(), 0, 0, w, this->get_Height());
		switch (this->state)
		{
			case 1:
				g->FillRectangle(ControlPaint::get_Dark(), 0, y1, w, y2 - y1);
				break;
			case 2:
				g->FillRectangle(ControlPaint::get_Dark(), 0, y3, w, y4 - y3);
				break;
		}
		g->Dispose();
		
		ScrollBar::OnPaint();
	}
	
	void VScrollBar::bar_MouseDown(int x, int y)
	{
		_P<ScrollBarButton> b = (ScrollBarButton*)this->bar.get();
		this->oldValue = this->value;
		this->pos1 = b->Bounds.Y;
		this->pos2 = this->pos1 + y;
	}
	
	void VScrollBar::bar_MouseMove(int x, int y)
	{
		int h = this->barRange - this->barWidth;
		_P<ScrollBarButton> b = (ScrollBarButton*)this->bar.get();
		if (h == 0 || !b->IsPushed) return;
		
		int my = b->Bounds.Y + y;
		int by = this->pos1 - DefaultSize * 2;
		if (-80 <= x && x < b->Bounds.Width + 80)
		{
			by += my - this->pos2;
		}
		int d = this->Maximum - this->Minimum;
		this->set_Value(this->Minimum + d * by / h);
	}
}}}
