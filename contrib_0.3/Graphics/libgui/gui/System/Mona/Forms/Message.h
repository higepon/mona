// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#ifndef __SYSTEM_MONA_FORMS_MESSAGE_H__
#define __SYSTEM_MONA_FORMS_MESSAGE_H__

namespace System { namespace Mona { namespace Forms
{
	struct Message
	{
	public:
		unsigned int from, header, arg1, arg2, arg3;
	};
}}}

#endif  // __SYSTEM_MONA_FORMS_MESSAGE_H__
