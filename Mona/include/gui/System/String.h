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
		
		wchar operator [](int index) const { return this->GetValue(index); }
		
		bool operator ==(String text);
		inline bool operator !=(String text) { return *this != text; }
		String& operator =(const String& text);
		String& operator =(const char* text);
		void operator +=(const String& text);
		
		bool StartsWith(const String& value);
		bool EndsWith(const String& value);
	};
}

#endif  // __SYSTEM_STRING_H__
