// This file is in the public domain.
// There are no restrictions on any sort of usage of this file.

#include <gui/System/Text/Encoding.h>

using namespace System;

namespace System { namespace Text
{
	_A<wchar> Encoding::ToWCharArray(const char* text, int length /*= -1*/)
	{
		int buflen = Encoding::GetLength(text, length);
		wchar* dest = new wchar[buflen];
		_A<wchar> ret(dest, buflen);
		int chlen;
		for (;;)
		{
			if (length < 0 && *text == '\0') break;
			*dest = Encoding::ToWChar(text, length);
			dest++;
			chlen = Encoding::GetCharLength(text, length);
			text += chlen;
			if (length > 0)
			{
				length -= chlen;
				if (length < 1) break;
			}
		}
		return ret;
	}
	
	wchar Encoding::ToWChar(const char* text, int max)
	{
		wchar c[6];
		
		if (max == 0 || *text == '\0') return 0;
		
		c[0] = (wchar)(unsigned char)*text;
		if (max == 1 || c[0] < 0xc0 || c[0] > 0xfd) return c[0];
		text++;
		
		c[1] = (wchar)(unsigned char)*text;
		if ((c[1] & 0xc0) != 0x80) return c[0];
		c[1] = c[1] & 0x3f;
		if ((c[0] & 0xe0) == 0xc0)
		{
			if (c[0] < 0xc2) return c[0];
			return ((c[0] & 0x1f) << 6) + c[1];
		}
		if (max == 2) return c[0];
		text++;
		
		c[2] = (wchar)(unsigned char)*text;
		if ((c[2] & 0xc0) != 0x80) return c[0];
		c[2] = c[2] & 0x3f;
		if ((c[0] & 0xf0) == 0xe0)
		{
			if (c[1] < 0x20 && c[0] < 0xe1) return c[0];
			return ((c[0] & 0x0f) << 12) + (c[1] << 6) + c[2];
		}
		if (max == 3) return c[0];
		text++;
		
		c[3] = (wchar)(unsigned char)*text;
		if ((c[3] & 0xc0) != 0x80) return c[0];
		c[3] = c[3] & 0x3f;
		if ((c[0] & 0xf8) == 0xf0)
		{
			if (c[1] < 0x10 && c[0] < 0xf1) return c[0];
			return ((c[1] & 0x0f) << 12) + (c[2] << 6) + c[3];
		}
		if (max == 4) return c[0];
		text++;
		
		c[4] = (wchar)(unsigned char)*text;
		if ((c[4] & 0xc0) != 0x80) return c[0];
		c[4] = c[4] & 0x3f;
		if ((c[0] & 0xfc) == 0xf8)
		{
			if (c[1] < 0x08 && c[0] < 0xf9) return c[0];
			return ((c[2] & 0x0f) << 12) + (c[3] << 6) + c[4];
		}
		if (max == 5) return c[0];
		text++;
		
		c[5] = (wchar)(unsigned char)*text;
		if ((c[5] & 0xc0) != 0x80) return c[0];
		c[5] = c[5] & 0x3f;
		if (c[1] < 0x04 && c[0] < 0xfd) return c[0];
		return ((c[3] & 0x0f) << 12) + (c[4] << 6) + c[5];
	}
	
	int Encoding::GetLength(const char* text, int length /*= -1*/)
	{
		if (length == 0) return 0;
		
		int ret = 0;
		int chlen;
		for (;;)
		{
			if (length < 0 && *text == '\0') break;
			ret++;
			chlen = Encoding::GetCharLength(text, length);
			text += chlen;
			if (length > 0)
			{
				length -= chlen;
				if (length < 1) break;
			}
		}
		return ret;
	}
	
	int Encoding::GetCharLength(const char* text, int max)
	{
		wchar c1;
		wchar c2;
		wchar ch;
		
		if (max == 0 || *text == '\0') return 1;
		
		c1 = (wchar)(unsigned char)*text;
		if (max == 1 || c1 < 0xc0 || c1 > 0xfd) return 1;
		text++;
		
		c2 = (wchar)(unsigned char)*text;
		if ((c2 & 0xc0) != 0x80) return 1;
		c2 = c2 & 0x3f;
		if ((c1 & 0xe0) == 0xc0)
		{
			if (c1 < 0xc2) return 1;
			return 2;
		}
		if (max == 2) return 1;
		text++;
		
		ch = (wchar)(unsigned char)*text;
		if ((ch & 0xc0) != 0x80) return 1;
		if ((c1 & 0xf0) == 0xe0)
		{
			if (c2 < 0x20 && c1 < 0xe1) return 1;
			return 3;
		}
		if (max == 3) return 1;
		text++;
		
		ch = (wchar)(unsigned char)*text;
		if ((ch & 0xc0) != 0x80) return 1;
		if ((c1 & 0xf8) == 0xf0)
		{
			if (c2 < 0x10 && c1 < 0xf1) return 1;
			return 4;
		}
		if (max == 4) return 1;
		text++;
		
		ch = (wchar)(unsigned char)*text;
		if ((ch & 0xc0) != 0x80) return 1;
		if ((c1 & 0xfc) == 0xf8)
		{
			if (c2 < 0x08 && c1 < 0xf9) return 1;
			return 5;
		}
		if (max == 5) return 1;
		text++;
		
		ch = (wchar)(unsigned char)*text;
		if ((ch & 0xc0) != 0x80) return 1;
		if (c2 < 0x04 && c1 < 0xfd) return 1;
		return 6;
	}
}}
