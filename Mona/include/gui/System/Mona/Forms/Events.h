// This file is in the public domain.
// There are no restrictions on any sort of usage of this file.

#ifndef __SYSTEM_MONA_FORMS_EVENTS_H__
#define __SYSTEM_MONA_FORMS_EVENTS_H__

#include <gui/System/Event.h>

namespace System { namespace Mona { namespace Forms
{
	enum MessageType { WM_MOUSEMOVE, WM_MOUSEDOWN, WM_MOUSEUP };
	
	class MouseEventArgs : public EventArgs
	{
	public:
		int Button, X, Y;
		
		virtual String get_TypeName() { return "System.Mona.Forms.MouseEventArgs"; }
		MouseEventArgs(int button, int x, int y) : Button(button), X(x), Y(y) {}
		virtual ~MouseEventArgs() {}
	};
	DECLARE_DELEGATE(MouseEventHandler, _P<MouseEventArgs>)
}}}

#endif  // __SYSTEM_MONA_FORMS_EVENTS_H__
