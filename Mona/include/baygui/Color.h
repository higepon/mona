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
	enum {
		DEFAULT_TRANSCOLOR = 0x00000000,
		DEFAULT_FORECOLOR  = 0xff000000,
		DEFAULT_BACKCOLOR  = 0xffc0c0c0,
	};
	
	//
	// システム標準16色に準拠
	//
	enum {
		BLACK     = 0xff000000,
		MAROON    = 0xff800000,
		GREEN     = 0xff008000,
		OLIVE     = 0xff808000,
		NAVY      = 0xff000080,
		PURPLE    = 0xff800080,
		TEAL      = 0xff008080,
		LIGHTGRAY = 0xffC0C0C0,
		GRAY      = 0xff808080,
		RED       = 0xffff0000,
		LIME      = 0xff00ff00,
		YELLOW    = 0xffffff00,
		BLUE      = 0xff0000ff,
		MAGENDA   = 0xffff00ff,
		CYAN      = 0xff00ffff,
		WHITE     = 0xffffffff,
	};
};

#endif /* _COLOR_H_INCLUDED_ */
