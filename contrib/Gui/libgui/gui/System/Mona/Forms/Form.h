// This file is in the public domain.
// There are no restrictions on any sort of usage of this file.

#ifndef __SYSTEM_MONA_FORMS_FORM_H__
#define __SYSTEM_MONA_FORMS_FORM_H__

#include <gui/System/Mona/Forms/Control.h>

namespace System { namespace Mona { namespace Forms
{
	class Form : public Control
	{
		friend class Control;
	
	protected:
		bool isCloseButtonPushed;
		NCState ncState;
	
	private:
		double opacity;
#ifdef MONA
		uint32_t overlap;
#endif
		System::Drawing::Point ptRevRect;
		_P<System::Drawing::Bitmap> formBuffer;
	
	public:
		static _P<Form> get_ActiveForm();
		virtual String get_TypeName() { return "System.Mona.Forms.Form"; }
		Form();
		virtual ~Form();
		
		virtual void Hide();
		virtual void Create();
		virtual void Dispose();
		
		bool CheckPoint(int x, int y);
		inline double get_Opacity() { return this->opacity; }
		void set_Opacity(double op);
	
	protected:
		virtual void Erase();
		virtual NCState NCHitTest(int x, int y);
#ifndef MONA
		void DrawReversibleRectangle();
#endif
		virtual void WndProc(uint32_t type, _P<EventArgs> e);
		virtual void OnPaint();
		virtual void OnNCMouseMove(_P<MouseEventArgs> e);
		virtual void OnNCMouseDown(_P<MouseEventArgs> e);
		virtual void OnNCMouseUp(_P<MouseEventArgs> e);
		
		virtual void OnActivated(_P<EventArgs> e);
		virtual void OnDeactivate(_P<EventArgs> e);
		
		DECLARE_EVENT(System::IEventHandler, Activated, _P<System::EventArgs>)
		DECLARE_EVENT(System::IEventHandler, Deactivate, _P<System::EventArgs>)
	};
}}}

#endif  // __SYSTEM_MONA_FORMS_FORM_H__
