/*
Copyright (c) 2004 bayside
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. The name of the author may not be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "baygui.h"

String& String::set(const char* s)
{
	unsigned char c1, c2, c3;
	int  slen;
	
	if (charArray) 
	{
		delete[] charArray;
		delete[] wcharArray;
	}
	if (s && *s)
	{
		wlen = 0;
		slen = strlen(s);
		charArray  = new char[slen + 1];
		wcharArray = new wchar[slen + 1];
		strcpy(charArray, s);
		
		// UTF-8 -> UCS-4
		for (int i = 0; i < slen; i++)
		{
			// 1st byte
			if (s[i] == 0) {
				break;
			} else {
				c1 = (unsigned char)s[i];
			}
			// 0aaa bbbb - > 0aaa bbbb (0x20-0x7F)
			if (c1 <= 0x7F)
			{
				wcharArray[wlen++] = c1;
			}
			// 110a aabb 10bb cccc -> 0000 0aaa bbbb cccc (0xC280-0xDFBF)
			else if (0xC2 <= c1 && c1 <= 0xDF)
			{
				// 2nd byte
				if (s[i] == slen - 1) {
					break;
				} else {
					c2 = (unsigned char)s[++i];
				}
				wcharArray[wlen++] = ((c1 & 0x1F) << 6) | (c2 & 0x3F);
			}
			// 1110 aaaa 10bb bbcc 10cc dddd -> aaaa bbbb cccc dddd (0xE0A080-0xEFBFBF)
			else if (0xE0 <= c1 && c1 <= 0xEF)
			{
				// 2nd byte
				if (s[i] == slen - 1) {
					break;
				} else {
					c2 = (unsigned char)s[++i];
				}
				// 3rd byte
				if (s[i] == slen - 1) {
					break;
				} else {
					c3 = (unsigned char)s[++i];
				}
				wcharArray[wlen++] = ((c1 & 0xF) << 12) | ((c2 & 0x3F) << 6) | (c3 & 0x3F);
			}
		}
	}
	else
	{
		charArray  = 0;
		wcharArray = 0;
		wlen = 0;
	}
	return *this;
}

String& String::set(int n)
{
	int i, j, k;
	char tmp, c20[20 + 1];
	
	if (charArray) 
	{
		delete[] charArray;
		delete[] wcharArray;
	}

	i = j = k = 0;
	memset(c20, 0, 21);
	if (n < 0) {
		n = -1 * n;
		i = 1;
	}
	if (n == 0) {
		c20[k++] = 0 + '0';
	}
	while (n != 0) {
		c20[k++] = n%10 + '0';
		n = n/10;
	}
	if (i == 1) c20[k++] = '-';
	c20[k] = '\0';

	i = j = 0;
	while (c20[j]) ++j;
	while (i < --j) {
		tmp = c20[i];
		c20[i++] = c20[j];
		c20[j] = tmp;
	}

	return set(c20);
}
