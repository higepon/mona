// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#ifndef __SYSTEM_MONA_FORMS_LABEL_H__
#define __SYSTEM_MONA_FORMS_LABEL_H__

#include <gui/System/Mona/Forms/Control.h>

namespace System { namespace Mona { namespace Forms
{
	class Label : public Control
	{
	public:
		virtual String get_TypeName() { return "System.Mona.Forms.Label"; }
		Label();
		virtual ~Label();
		
	protected:
		virtual void OnPaint();
	};
}}}

#endif  // __SYSTEM_MONA_FORMS_LABEL_H__
