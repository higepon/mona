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

#if !defined(_SCROLLBAR_H_INCLUDED_)
#define _SCROLLBAR_H_INCLUDED_

namespace baygui {
	/**
	 水平・垂直スクロールバークラス
	*/
	class Scrollbar : public Component {
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
		/** 一つ前の値 */
		int preValue;
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

		/** 最小値を得る（初期値は0） */
		inline int getMinimum() { return this->minimum; }

		/** 最大値を得る（初期値は100） */
		inline int getMaximum() { return this->maximum; }

		/** 一度に増減する量を得る（初期値は10） */
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
		virtual void setBounds(int x, int y, int w, int h);

		/** 描画ハンドラ */
		virtual void paint(Graphics* g);

		/** イベントハンドラ */
		virtual void processEvent(Event* event);
	};
}

#endif /* _SCROLLBAR_H_INCLUDED_ */
