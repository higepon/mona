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
	 色構造体（システム標準16色に準拠）
	 */
	struct Color {
	public:
		/** 黒色 */
		static const int black     = 0xff000000;
		/** 濃い茶色 */
		static const int maroon    = 0xff800000;
		/** 濃い緑色 */
		static const int green     = 0xff008000;
		/** 濃い黄色 */
		static const int olive     = 0xff808000;
		/** 濃い青色 */
		static const int navy      = 0xff000080;
		/** 紫色 */
		static const int purple    = 0xff800080;
		/** 暗い青色 */
		static const int teal      = 0xff008080;
		/** 明るい灰色 */
		static const int lightGray = 0xffC0C0C0;
		/** 灰色 */
		static const int gray      = 0xff808080;
		/** 赤色 */
		static const int red       = 0xffff0000;
		/** 明るい緑色 */
		static const int lime      = 0xff00ff00;
		/** 黄色 */
		static const int yellow    = 0xffffff00;
		/** 青色 */
		static const int blue      = 0xff0000ff;
		/** 明るい赤色 */
		static const int magenta   = 0xffff00ff;
		/** 水色 */
		static const int cyan      = 0xff00ffff;
		/** 白色 */
		static const int white     = 0xffffffff;
	};
}

#endif /* _COLOR_H_INCLUDED_ */
