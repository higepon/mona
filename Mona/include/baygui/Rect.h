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
