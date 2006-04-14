// This file is in the public domain.
// There are no restrictions on any sort of usage of this file.

#ifndef __SYSTEM_MONA_FORMS_HSCROLLBAR_H__
#define __SYSTEM_MONA_FORMS_HSCROLLBAR_H__

#include "ScrollBar.h"

namespace System { namespace Mona { namespace Forms
{
	class HScrollBar : public ScrollBar
	{
	public:
		virtual String get_TypeName() { return "System.Mona.Forms.HScrollBar"; }
		HScrollBar();
		virtual ~HScrollBar();
		
		virtual void Create();
		virtual void SetButton();
		virtual void SetState(int x, int y);
		virtual void OnPaint();

	protected:
		virtual void bar_MouseDown(int x, int y);
		virtual void bar_MouseMove(int x, int y);
	};
}}}

#endif  // __SYSTEM_MONA_FORMS_HSCROLLBAR_H__
