// This file is in the public domain.
// There are no restrictions on any sort of usage of this file.

#ifndef __SYSTEM_STRING_H__
#define __SYSTEM_STRING_H__

#include <gui/System/Text/Encoding.h>

namespace System
{
	struct String : public _A<wchar>
	{
		String();
		String(const char* text, int length = -1);
		String(_A<wchar> text);
		virtual ~String();
		
		bool operator ==(String text);
		inline bool operator !=(String text) { return *this != text; }
		String& operator =(const String& text);
		String& operator =(const char* text);
		void operator +=(const String& text);
	};
}

#endif  // __SYSTEM_STRING_H__
