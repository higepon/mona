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

/** コンストラクタ */
Font::Font()
{
}

/** デストラクタ */
Font::~Font()
{
}

/** 名前を設定する */
void Font::setName(char *name)
{
	xstrncpy(this->name, name, 8 + 1);
}

/** 幅を設定する */
void Font::setWidth(int width)
{
	this->width = width;
}

/** 高さを設定する */
void Font::setHeight(int height)
{
	this->height = height;
}

/** 書体を設定する */
void Font::setStyle(int style)
{
	this->style = style;
}

/** 名前を得る */
char *Font::getName()
{
	return name;
}

/** 幅を得る */
int Font::getWidth()
{
	return width;
}

/** 高さを得る */
int Font::getHeight()
{
	return height;
}

/** 書体を得る */
int Font::getStyle()
{
	return style;
}

/** 内部データを得る */
unsigned char *Font::getData(){
	return data;
}
