// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#ifndef __SYSTEM_MONA_FORMS_IMESSAGEFILTER_H__
#define __SYSTEM_MONA_FORMS_IMESSAGEFILTER_H__

#include <gui/System/Mona/Forms/Message.h>

namespace System { namespace Mona { namespace Forms
{
	class IMessageFilter
	{
	public:
		virtual ~IMessageFilter(){};
		virtual bool PreFilterMessage(Message* m) = 0;
	};
}}}

#endif  // __SYSTEM_MONA_FORMS_IMESSAGEFILTER_H__
