// This file is in the public domain.
// There are no restrictions on any sort of usage of this file.

#ifndef __SYSTEM_MONA_FORMS_SCROLLBAR_H__
#define __SYSTEM_MONA_FORMS_SCROLLBAR_H__

#include <gui/System/Mona/Forms/Control.h>
#include <gui/System/Mona/Forms/Timer.h>

namespace System { namespace Mona { namespace Forms
{
	class ScrollBar : public Control
	{
	public:
		static const int DefaultSize = 16;
	
	protected:
		_P<Object> bar;
		_P<Control> bt1, bt2;
		int barRange, barPos, barWidth, state, oldValue, pos1, pos2;
		_P<Timer> timer1;
		System::Drawing::Point mouse;
		int value;
		bool isBarTarget;
		
	public:
		int Minimum, Maximum, SmallChange, LargeChange;

		virtual String get_TypeName() { return "System.Mona.Forms.ScrollBar"; }
		ScrollBar();
		virtual ~ScrollBar();
		
		inline int get_Value() { return this->value; }
		void set_Value(int value);
		
		virtual void Create();
		virtual void Dispose();
		virtual void SetButton() = 0;
		
	protected:
		void Calculate(int width);
		virtual void SetState(int x, int y) = 0;
		virtual void OnPaint();
		virtual void OnMouseDown(_P<MouseEventArgs> e);
		virtual void OnMouseMove(_P<MouseEventArgs> e);
		virtual void OnMouseUp(_P<MouseEventArgs> e);
		virtual void OnClick(_P<EventArgs> e);
		virtual void OnValueChanged(_P<EventArgs> e);
		virtual void bar_MouseDown(int x, int y) = 0;
		virtual void bar_MouseMove(int x, int y) = 0;
		void SetButtonInternal(int x, int y, int w, int h);
		
	private:
		void dec_Click(_P<Object> sender, _P<EventArgs> e);
		void inc_Click(_P<Object> sender, _P<EventArgs> e);
		void timer1_Tick(_P<Object> sender, _P<EventArgs> e);
		
		DECLARE_EVENT(System::IEventHandler, ValueChanged, _P<EventArgs>)
	};
}}}

#endif  // __SYSTEM_MONA_FORMS_SCROLLBAR_H__
