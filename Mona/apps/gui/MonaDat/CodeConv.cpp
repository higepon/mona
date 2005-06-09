// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#include <gui/System/String.h>
#include "../../../tools/code_table/cp932.h"

using namespace System;

bool StringCompare(const char* data, int offset, int len, const char* str)
{
	for (int i = 0; i < len && *str != '\0' && data[offset] == *str; i++, offset++, str++);
	return *str == '\0';
}

static wchar CheckEntity(const char* data, int offset, int len)
{
	if (StringCompare(data, offset, len, "&nbsp;")) return ' ';
	if (StringCompare(data, offset, len, "&gt;"  )) return '>';
	if (StringCompare(data, offset, len, "&lt;"  )) return '<';
	if (StringCompare(data, offset, len, "&amp;" )) return '&';
#if 0
	// result: &instancedata; &hearts; &heart; &rlo; &lro; &a; &ntilde;
	bool ok = true;
	int i = 1;
	for (; i < len; i++)
	{
		char ch = data[offset + i];
		if (ch == ';') break;
		if ((ch < 'A' || ch > 'Z') && (ch < 'a' || ch > 'z'))
		{
			ok = false;
			break;
		}
	}
	if (ok)
	{
		for (int j = 0; j <= i; j++) printf("%c", data[offset + j]);
		printf("\n");
	}
#endif
	return '\0';
}

String StringDecode(const char* data, int offset, int len)
{
	int wlen = 0;
	for (int i = 0; i < len; i++, wlen++)
	{
		if (data[offset + i] == '&' && CheckEntity(data, offset + i, len - i) != '\0')
		{
			for (; i < len && data[offset + i] != ';'; i++);
			continue;
		}
		
		unsigned char c1 = (unsigned char)data[offset + i];
		if ((0x81 <= c1 && c1 <= 0x9f) || (0xe0 <= c1 && c1 <= 0xfc)) i++;
	}
	_A<wchar> ret(wlen);
	int p = 0;
	for (int i = 0; i < len; i++, p++)
	{
		wchar ch;
		if (data[offset + i] == '&' && (ch = CheckEntity(data, offset + i, len - i)) != '\0')
		{
			for (; i < len && data[offset + i] != ';'; i++);
			ret[p] = ch;
			continue;
		}
		
		unsigned char c1 = (unsigned char)data[offset + i];
		if (c1 < 0x80)
		{
			ret[p] = c1;
		}
		else if (c1 == 0x80 || c1 == 0xa0 || c1 >= 0xfd)
		{
			ret[p] = '?';
		}
		else if (0xa1 <= c1 && c1 <= 0xdf)
		{
			ret[p] = c1 + 0xfec0;
		}
		else
		{
			i++;
			unsigned char c2 = (unsigned char)data[offset + i];
			if(c2 < 0x40 || c2 == 0x7f || c2 > 0xfc)
			{
				ret[p] = '?';
			}
			if (c1 >= 0xe0) c1 -= 0x40;
			if (c2 > 0x7e) c2--;
			ret[p] = table_cp932[(c1 - 0x81) * 188 + (c2 - 0x40)];
			if (ret[p] == 0) ret[p] = '?';
		}
	}
	return ret;
}
