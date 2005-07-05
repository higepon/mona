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

#if !defined(_COLOR_H_INCLUDED_)
#define _COLOR_H_INCLUDED_

/**
 色構造体
 */
struct Color {
public:
	//
	// 部品の初期色
	//
	static const int DEFAULT_TRANSCOLOR = 0x00000000;
	static const int DEFAULT_FORECOLOR  = 0xff000000;
	static const int DEFAULT_BACKCOLOR  = 0xffc0c0c0;
	
	//
	// システム標準16色に準拠
	//
	static const int BLACK     = 0xff000000;
	static const int MAROON    = 0xff800000;
	static const int GREEN     = 0xff008000;
	static const int OLIVE     = 0xff808000;
	static const int NAVY      = 0xff000080;
	static const int PURPLE    = 0xff800080;
	static const int TEAL      = 0xff008080;
	static const int LIGHTGRAY = 0xffC0C0C0;
	static const int GRAY      = 0xff808080;
	static const int RED       = 0xffff0000;
	static const int LIME      = 0xff00ff00;
	static const int YELLOW    = 0xffffff00;
	static const int BLUE      = 0xff0000ff;
	static const int MAGENDA   = 0xffff00ff;
	static const int CYAN      = 0xff00ffff;
	static const int WHITE     = 0xffffffff;
};

#endif /* _COLOR_H_INCLUDED_ */
