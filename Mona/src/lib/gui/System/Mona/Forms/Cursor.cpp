// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#include <gui/System/Mona/Forms/Cursor.h>

#ifdef MONA
#include <monapi/messages.h>

extern CommonParameters* __commonParams;
#endif

using namespace System;
using namespace System::Drawing;

namespace System { namespace Mona { namespace Forms
{
	Point Cursor::get_Position()
	{
#ifdef MONA
		return Point(__commonParams->mouse.x, __commonParams->mouse.y);
#else
		/// needs to implement!!!
		return Point::get_Empty();
#endif
	}
}}}
