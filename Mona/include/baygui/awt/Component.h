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

#if !defined(_COMPONENT_H_INCLUDED_)
#define _COMPONENT_H_INCLUDED_

namespace baygui {
	class Container;

	/**
	 部品クラス
	 */
	class Component : public Object {
	private:
		/** 親部品 */
		Container* parent;
		/** 活性・非活性 */
		bool enabled;
		/** フォーカス有無 */
		bool focused;
		/** 表示・非表示 */
		bool visible;
		/** x（絶対座標） */
		int x;
		/** y（絶対座標） */
		int y;
		/** 高さ */
		int height;
		/** 幅 */
		int width;
		/** 背景色 */
		dword backColor;
		/** 前景色 */
		dword foreColor;
		/** フォントスタイル */
		int fontStyle;
		/** 部品の大きさ */
		Rectangle bounds;
		/** 描画領域 */
		Graphics* _g;
		/** 描画バッファー */
		Image* _buffer;
		/** フォント情報 */
		FontMetrics* _metrics;

	protected:
		/** フォーカスイベント */
		Event focusEvent;

	public:
		/** コンストラクタ */
		Component();
		
		/** デストラクタ */
		virtual ~Component();
		
		/**
		 部品生成ハンドラ.
		 後でremoveNotify()呼ぶと再初期化できる。
		 */
		virtual void addNotify();
		
		/**
		 部品破棄ハンドラ.
		 後でaddNotify()を呼ぶと再初期化できる。
		*/
		virtual void removeNotify();

		/** イベントハンドラ */
		virtual void processEvent(Event* event);
		
		/** 描画ハンドラ */
		virtual void paint(Graphics* g);
		
		/** イベント処理 */
		virtual void dispatchEvent(Event* event);
		
		/** 再描画 */
		virtual void repaint();
		
		/** 部品更新 */
		virtual void update();
		
		/** 親部品を得る */
		inline Container* getParent() { return this->parent; }
		
		/** 活性・非活性を得る */
		inline bool getEnabled() { return this->enabled; }
		
		/** フォーカス状態を得る */
		inline bool getFocused() { return this->focused; }
		
		/** 表示・非表示を得る */
		inline bool getVisible() { return this->visible; }
		
		/** X座標を得る */
		inline int getX() { return this->x; }
		
		/** Y座標を得る */
		inline int getY() { return this->y; }
		
		/** 幅を得る */
		inline int getWidth() { return this->width; }
		
		/** 高さを得る */
		inline int getHeight() { return this->height; }
		
		/** 部品の大きさ */
		inline Rectangle* getBounds() { return &this->bounds; }
		
		/** 背景色を得る */
		inline dword getBackground() { return this->backColor; }
		
		/** 前景色を得る */
		inline dword getForeground() { return this->foreColor; }
		
		/** フォントスタイルを得る */
		inline int getFontStyle() { return this->fontStyle; }
		
		/** 描画オブジェクトを得る */
		virtual Graphics* getGraphics() { return this->_g; }
		
		/**
		 内部バッファーを得る.
		 VRAMのように直接描画したいときに用いる。
		 Graphicsクラスが持っているような便利なメソッドはない。
		 描画後update()を呼ぶことで実際の描画がされる。
		 */
		virtual Image* getBuffer() { return this->_buffer; }
		
		/** フォント情報を得る */
		inline FontMetrics* getFontMetrics() { return this->_metrics; }
		
		/**
		 メインウィンドウを得る.
		 メインウィンドウとは全ての部品が乗っている一番下のウィンドウ。
		 メインウィンドウは各アプリケーションごとに必ず１つ存在している。
		 */
		virtual Component* getMainWindow();
		
		/**
		 活性状態を設定する
		 @param enabled 活性状態 (true / false)
		 */
		virtual void setEnabled(bool enabled);
		
		/**
		 フォーカス状態を設定する.
		 このメソッドを呼ぶと該当部品にFOCUS_INまたはFOCUS_OUTイベントが投げられる。
		 @param focused フォーカス状態 (true / false)
		 */
		virtual void setFocused(bool focused);
		
		/**
		 表示状態を設定する
		 @param visible 表示状態 (true / false)
		 */
		virtual void setVisible(bool visible);
		
		/**
		 大きさを設定する
		 @param x x座標
		 @param y y座標
		 @param width 幅
		 @param height 高さ
		*/
		virtual void setBounds(int x, int y, int width, int height);
		
		/**
		 位置を変更する
		 @param x X座標
		 @param y Y座標
		*/
		virtual void setLocation(int x, int y);
		
		/**
		 親部品を設定する
		 @param parent 親部品
		*/
		virtual void setParent(Container* parent);
		
		/** 背景色を設定する */
		virtual void setBackground(dword backColor);
		
		/** 前景色を設定する */
		virtual void setForeground(dword foreColor);
		
		/**
		 フォントスタイル（通常、太字、斜字、固定幅）を設定する.
		 各スタイルを"|"で組み合わせることも可能。
		 @param style フォントスタイル (Font::PLAIN / Font::BOLD / Font::ITALIC / Font::FIXED )
		*/
		virtual void setFontStyle(int style);
	};
}

#endif // _COMPONENT_H_INCLUDED_
