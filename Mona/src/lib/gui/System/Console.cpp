// This file is in the public domain.
// There are no restrictions on any sort of usage of this file.

#ifdef MONA
#include <monapi.h>
#else
#include <stdio.h>
#endif
#include <gui/System/Console.h>

namespace System
{
	void Console::Write(String text)
	{
		int len = text.get_Length();
		char* buf = new char[len + 1];
		for (int i = 0; i < len; i++)
		{
			wchar ch = text[i];
			buf[i] = ch < 128 ? (char)ch : '?';
		}
		buf[len] = '\0';
		::printf(buf);
		delete [] buf;
	}
	
	void Console::WriteLine(String text)
	{
		Console::Write(text);
		::printf("\n");
	}
}
