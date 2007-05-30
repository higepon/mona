// This file is in the public domain.
// There are no restrictions on any sort of usage of this file.

#ifndef __SYSTEM_MONA_FORMS_SCROLLABLECONTROL_H__
#define __SYSTEM_MONA_FORMS_SCROLLABLECONTROL_H__

#include <gui/System/Mona/Forms/HScrollBar.h>
#include <gui/System/Mona/Forms/VScrollBar.h>

namespace System { namespace Mona { namespace Forms
{
	class ScrollableControl : public Control
	{
	protected:
		System::Drawing::Rectangle area;
		_P<HScrollBar> hScrollBar;
		_P<VScrollBar> vScrollBar;
		int status;
		
	public:
		virtual String get_TypeName() { return "System.Mona.Forms.ScrollableControl"; }
		ScrollableControl();
		virtual ~ScrollableControl();
		
		System::Drawing::Point get_ScrollPosition();
		void set_ScrollPosition(System::Drawing::Point p);
		virtual _P<System::Drawing::Graphics> CreateGraphics();
		inline System::Drawing::Rectangle get_ScrollArea() { return this->area; }
		void set_ScrollArea(System::Drawing::Rectangle value);
		virtual void Create();
		
	protected:
		virtual void SetScrollBarPosition();
		virtual void OnPaint();
		virtual void OnScroll(_P<EventArgs> e);
		
	private:
		void scrollBar_ValueChanged(_P<Object> sender, _P<EventArgs> e);
		
		DECLARE_EVENT(System::IEventHandler, Scroll, _P<EventArgs>)
	};
}}}

#endif  // __SYSTEM_MONA_FORMS_SCROLLABLECONTROL_H__
