/*
Copyright (c) 2005 bayside

Permission is hereby granted, free of charge, to any person 
obtaining a copy of this software and associated documentation files 
(the "Software"), to deal in the Software without restriction, 
including without limitation the rights to use, copy, modify, merge, 
publish, distribute, sublicense, and/or sell copies of the Software, 
and to permit persons to whom the Software is furnished to do so, 
subject to the following conditions:

The above copyright notice and this permission notice shall be 
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY 
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "baygui.h"

String& String::set(const char* s)
{
	unsigned char c1, c2, c3;
	int  slen;
	
	if (bytes) {
		delete[] bytes;
		delete[] wcharArray;
	}
	if (s && *s) {
		len = 0;
		slen = strlen(s);
		bytes  = new char[slen + 1];
		wcharArray = new wchar[slen + 1];
		strcpy(bytes, s);
		
		// UTF-8 -> UCS-4
		for (int i = 0; i < slen; i++) {
			// 1st byte
			if (s[i] == 0) {
				break;
			} else {
				c1 = (unsigned char)s[i];
			}
			// 0aaa bbbb - > 0aaa bbbb (0x20-0x7F)
			if (c1 <= 0x7F) {
				wcharArray[len++] = c1;
			}
			// 110a aabb 10bb cccc -> 0000 0aaa bbbb cccc (0xC280-0xDFBF)
			else if (0xC2 <= c1 && c1 <= 0xDF) {
				// 2nd byte
				if (s[i] == slen - 1) {
					break;
				} else {
					c2 = (unsigned char)s[++i];
				}
				wcharArray[len++] = ((c1 & 0x1F) << 6) | (c2 & 0x3F);
			}
			// 1110 aaaa 10bb bbcc 10cc dddd -> aaaa bbbb cccc dddd (0xE0A080-0xEFBFBF)
			else if (0xE0 <= c1 && c1 <= 0xEF) {
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
				wcharArray[len++] = ((c1 & 0xF) << 12) | ((c2 & 0x3F) << 6) | (c3 & 0x3F);
			}
		}
	} else {
		bytes  = 0;
		wcharArray = 0;
		len = 0;
	}
	return *this;
}

String& String::set(int n)
{
	int i, j, k;
	char tmp, c20[20 + 1];
	
	if (bytes) {
		delete[] bytes;
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

bool String::equals(const char *s)
{
	if (s == NULL) return false;
	if (strcmp(bytes, s) == 0) {
		return true;
	} else {
		return false;
	}
}

bool String::startsWith(const char* s)
{
	if (s == 0 || len == 0 || (int)strlen(s) > (int)strlen(bytes)) return false;
	for (int i = 0; i < (int)strlen(s); i++) {
		if (s[i] != bytes[i]) return false;
	}
	return true;
}

bool String::endsWith(const char* s)
{
	if (s == 0 || len == 0 || (int)strlen(s) > (int)strlen(bytes)) return false;
	for (int i = 0; i < (int)strlen(s); i++) {
		if (s[(int)strlen(s) - i - 1] != 
			bytes[(int)strlen(bytes) - i - 1]) return false;
	}
	return true;
}

