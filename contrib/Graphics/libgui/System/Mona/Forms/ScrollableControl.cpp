// This file is in the public domain.
// There are no restrictions on any sort of usage of this file.

#include <gui/System/Math.h>
#include <gui/System/Mona/Forms/ControlPaint.h>
#include <gui/System/Mona/Forms/ScrollableControl.h>

using namespace System;
using namespace System::Drawing;

namespace System { namespace Mona { namespace Forms
{
	ScrollableControl::ScrollableControl() : status(0)
	{
		this->hScrollBar = new HScrollBar();
		this->vScrollBar = new VScrollBar();
		this->hScrollBar->SmallChange = Control::get_DefaultFont()->get_Height();
		this->vScrollBar->SmallChange = Control::get_DefaultFont()->get_Height();
		this->hScrollBar->add_ValueChanged(new EventHandler<ScrollableControl>(this, &ScrollableControl::scrollBar_ValueChanged));
		this->vScrollBar->add_ValueChanged(new EventHandler<ScrollableControl>(this, &ScrollableControl::scrollBar_ValueChanged));
	}
	
	ScrollableControl::~ScrollableControl()
	{
	}
	
	_P<System::Drawing::Graphics> ScrollableControl::CreateGraphics()
	{
		_P<Graphics> ret = Graphics::FromImage(this->buffer);
		Size sz = this->get_ClientSize();
		int ds = ScrollBar::DefaultSize;
		ret->set_ClientRectangle(Rectangle(2, 2, sz.Width - ds - 4, sz.Height - ds - 4));
		return ret;
	}
	
	Point ScrollableControl::get_ScrollPosition()
	{
		return Point(this->hScrollBar->get_Value(), this->vScrollBar->get_Value());
	}
	
	void ScrollableControl::set_ScrollPosition(System::Drawing::Point p)
	{
		this->status = 1;
		this->hScrollBar->set_Value(p.X);
		this->vScrollBar->set_Value(p.Y);
		this->status = 0;
		this->Refresh();
	}
	
	void ScrollableControl::set_ScrollArea(Rectangle value)
	{
		this->status = 1;
		this->area = value;
		
		Size sz = this->get_ClientSize();
		sz.Width  -= ScrollBar::DefaultSize + 4;
		sz.Height -= ScrollBar::DefaultSize + 4;
		
		this->hScrollBar->Minimum = value.X;
		this->hScrollBar->Maximum = value.get_Right() - sz.Width;
		this->hScrollBar->LargeChange = sz.Width;
		this->hScrollBar->SetButton();
		
		this->vScrollBar->Minimum = value.Y;
		this->vScrollBar->Maximum = value.get_Bottom() - sz.Height;
		this->vScrollBar->LargeChange = sz.Height;
		this->vScrollBar->SetButton();
		
		this->status = 0;
	}
	
	void ScrollableControl::Create()
	{
		this->SetScrollBarPosition();
		this->get_Controls()->Add(this->hScrollBar.get());
		this->get_Controls()->Add(this->vScrollBar.get());
		
		Control::Create();
	}
	
	void ScrollableControl::SetScrollBarPosition()
	{
		Size sz = this->get_ClientSize();
		int ds = ScrollBar::DefaultSize;
		this->hScrollBar->set_Bounds(Rectangle(0, sz.Height - ds, sz.Width - ds, ds));
		this->vScrollBar->set_Bounds(Rectangle(sz.Width - ds, 0, ds, sz.Height - ds));
	}
	
	void ScrollableControl::OnPaint()
	{
		Control::OnPaint();
		
		_P<Graphics> g = Graphics::FromImage(this->buffer);
		int ds = ScrollBar::DefaultSize;
		int w = this->get_Width() - ds, h = this->get_Height() - ds;
		ControlPaint::DrawSunken(g, 0, 0, w, h);
		g->Dispose();
	}
	
	void ScrollableControl::OnScroll(_P<EventArgs> e)
	{
		this->raise_Scroll(this, e);
	}
	
	void ScrollableControl::scrollBar_ValueChanged(_P<Object> sender, _P<EventArgs> e)
	{
		if (this->status == 0)
		{
			this->OnScroll(EventArgs::get_Empty());
		}
		else
		{
			this->status++;
		}
	}
}}}
