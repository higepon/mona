// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#ifndef __SYSTEM_MONA_FORMS_BUTTON_H__
#define __SYSTEM_MONA_FORMS_BUTTON_H__

#include <gui/System/Mona/Forms/Control.h>

namespace System { namespace Mona { namespace Forms
{
	class Button : public Control
	{
	protected:
		bool isPushed;
		
	public:
		virtual String get_TypeName() { return "System.Mona.Forms.Button"; }
		Button();
		virtual ~Button();
		
		virtual void Create();
		
	protected:
		virtual void OnPaint();
		virtual void OnMouseMove(_P<MouseEventArgs> e);
		virtual void OnMouseDown(_P<MouseEventArgs> e);
		virtual void OnMouseUp(_P<MouseEventArgs> e);
	};
}}}

#endif  // __SYSTEM_MONA_FORMS_BUTTON_H__
