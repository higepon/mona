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
