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

#if !defined(_RECT_H_INCLUDED_)
#define _RECT_H_INCLUDED_

/**
 矩形クラス
*/
class Rect : public Object {
private:
	/** x（絶対座標）*/
	int x;
	/** y（絶対座標）*/
	int y;
	/** 高さ */
	int height;
	/** 幅 */
	int width;
	
public:
	/** デフォルトコンストラクタ */
	Rect() { this->x = this->y = this->width = this->height = 0; }
	
	/** デストラクタ */
	virtual ~Rect() {}
	
	/** X座標を得る */
	inline int getX() { return this->x; }
	
	/** Y座標を得る */
	inline int getY() { return this->y; }
	
	/** 幅を得る */
	inline int getWidth() { return this->width; }
	
	/** 高さを得る */
	inline int getHeight() { return this->height; }
	
	/** 位置を設定する */
	inline void setLocation(int x, int y) { this->x = x; this->y = y; }
	
	/** 大きさを設定する */
	inline void setSize(int w, int h) { this->width = w; this->height = h; }
};

#endif // _RECT_H_INCLUDED_
