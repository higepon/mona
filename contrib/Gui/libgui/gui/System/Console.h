// This file is in the public domain.
// There are no restrictions on any sort of usage of this file.

#ifndef __SYSTEM_CONSOLE_H__
#define __SYSTEM_CONSOLE_H__

#include <gui/System/String.h>

namespace System
{
	class Console
	{
	public:
		static void Write(String text);
		static void WriteLine(String text);
	};
}

#endif  // __SYSTEM_CONSOLE_H__
