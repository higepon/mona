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

namespace baygui {
	/**
	 色構造体
	 */
	struct Color {
	public:
		//
		// システム標準16色に準拠
		//
		/** <font color="#000000">黒色</font> */
		static const int black     = 0xff000000;
		/** <font color="#800000">濃い茶色</font> */
		static const int maroon    = 0xff800000;
		/** <font color="#008000">濃い緑色</font> */
		static const int green     = 0xff008000;
		/** <font color="#808000">濃い黄色</font> */
		static const int olive     = 0xff808000;
		/** <font color="#000080">濃い青色</font> */
		static const int navy      = 0xff000080;
		/** <font color="#800080">紫色</font> */
		static const int purple    = 0xff800080;
		/** <font color="#008080">暗い青色</font> */
		static const int teal      = 0xff008080;
		/** <font color="#c0c0c0">明るい灰色</font> */
		static const int lightGray = 0xffC0C0C0;
		/** <font color="#808080">灰色</font> */
		static const int gray      = 0xff808080;
		/** <font color="#ff0000">赤色</font> */
		static const int red       = 0xffff0000;
		/** <font color="#00ff00">明るい緑色</font> */
		static const int lime      = 0xff00ff00;
		/** <font color="#ffff00">黄色</font> */
		static const int yellow    = 0xffffff00;
		/** <font color="#0000ff">青色</font> */
		static const int blue      = 0xff0000ff;
		/** <font color="#ff00ff">明るい赤色</font> */
		static const int magenta   = 0xffff00ff;
		/** <font color="#00ffff">水色</font> */
		static const int cyan      = 0xff00ffff;
		/** <font color="#000000">白色</font> */
		static const int white     = 0xffffffff;
	};
}

#endif /* _COLOR_H_INCLUDED_ */
