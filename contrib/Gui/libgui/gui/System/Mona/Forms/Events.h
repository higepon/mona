// This file is in the public domain.
// There are no restrictions on any sort of usage of this file.

#ifndef __SYSTEM_MONA_FORMS_EVENTS_H__
#define __SYSTEM_MONA_FORMS_EVENTS_H__

#include <gui/System/Event.h>

namespace System { namespace Mona { namespace Forms
{
	class MouseEventArgs : public EventArgs
	{
	public:
		int Button, X, Y;
		
		virtual String get_TypeName() { return "System.Mona.Forms.MouseEventArgs"; }
		MouseEventArgs(int button, int x, int y) : Button(button), X(x), Y(y) {}
		virtual ~MouseEventArgs() {}
	};
	DECLARE_DELEGATE(MouseEventHandler, _P<MouseEventArgs>)
	
	class KeyEventArgs : public EventArgs
	{
	public:
		unsigned int KeyCode, Modifiers;
		
		virtual String get_TypeName() { return "System.Mona.Forms.KeyEventArgs"; }
		KeyEventArgs(unsigned int keyCode, unsigned int modifiers): KeyCode(keyCode), Modifiers(modifiers) {}
		virtual ~KeyEventArgs() {}
	};
	DECLARE_DELEGATE(KeyEventHandler, _P<KeyEventArgs>)
}}}

#endif  // __SYSTEM_MONA_FORMS_EVENTS_H__
