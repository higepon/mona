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

#if !defined(_SCROLLBAR_H_INCLUDED_)
#define _SCROLLBAR_H_INCLUDED_

class Scrollbar : public Control {
public:
	/** 水平スクロールバー */
	static const int HORIZONTAL = 0;
	/** 垂直スクロールバー */
	static const int VERTICAL   = 1;
	
private:
	/** スクロールバーの種類 */
	int orientation;
	/** 最小値 */
	int minimum;
	/** 最大値 */
	int maximum;
	/** 一度に増減する量 */
	int blocksize;
	/** 現在の値 */
	int value;
	/** ブロックイベント */
	Event blockEvent;
	
public:
	/** デフォルトコンストラクタ */
	Scrollbar();

	/** コンストラクタ */
	Scrollbar(int orientation);

	/** デストラクタ */
	virtual ~Scrollbar();

	/** 最小値を得る */
	inline int getMinimum() { return this->minimum; }

	/** 最大値を得る */
	inline int getMaximum() { return this->maximum; }

	/** 一度に増減する量を得る */
	inline int getBlocksize() { return this->blocksize; }

	/** 値を得る */
	inline int getValue() { return this->value; }

	/** 最小値を設定する */
	inline void setMinimum(int n) { this->minimum = n; }

	/** 最大値を設定する */
	inline void setMaximum(int n) { this->maximum = n; }

	/** 一度に増減する量を設定する */
	inline void setBlocksize(int n) { this->blocksize = n; }

	/** 値を設定する */
	virtual void setValue(int value);

	/**
	 部品の大きさを設定する.
	 垂直スクロールバーの時は幅16固定、水平スクロールバーのときは高さ16固定。
	 それ以外の値を設定しても16になる。
	*/
	virtual void setRect(int x, int y, int w, int h);

	/** 描画ハンドラ */
	virtual void onPaint(Graphics *g);

	/** イベントハンドラ */
	virtual void onEvent(Event *event);
};

#endif /* _SCROLLBAR_H_INCLUDED_ */
