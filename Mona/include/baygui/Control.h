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

#if !defined(_CONTROL_H_INCLUDED_)
#define _CONTROL_H_INCLUDED_

class Container;

/**
 部品クラス
 */
class Control : public Object {
public:
	//
	// 配置
	//
	enum {
		/** 左寄せ */
		ALIGN_LEFT       = 1,
		/** 中央寄せ */
		ALIGN_CENTER     = 2,
		/** 右寄せ */
		ALIGN_RIGHT      = 3,
	};

private:
	/** 親部品 */
	Container *parent;
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
	unsigned int backColor;
	/** 前景色 */
	unsigned int foreColor;
	/** フォントスタイル */
	int fontStyle;
	/** 部品の大きさ */
	Rect rect;
	/** 描画領域 */
	Graphics *_g;
	/** 描画バッファー */
	Image *_buffer;
	/** フォント情報 */
	FontMetrics *_metrics;

protected:
	/** フォーカスイベント */
	Event focusEvent;

public:
	/** コンストラクタ */
	Control::Control();
	
	/** デストラクタ */
	virtual Control::~Control();
	
	/**
	 部品生成ハンドラ.
	 後でonExit()呼ぶと再初期化できる。
	 */
	virtual void onStart();
	
	/**
	 部品破棄ハンドラ.
	 後でonStart()を呼ぶと再初期化できる。
	*/
	virtual void onExit();

	/** イベントハンドラ */
	virtual void onEvent(Event *event);
	
	/** 描画ハンドラ */
	virtual void onPaint(Graphics *g);
	
	/** イベント処理 */
	virtual void postEvent(Event *event);
	
	/**
	 再描画.
	 この中でonPaint()とupdate()を呼ぶ。
	 */
	virtual void repaint();
	
	/** 部品更新 */
	virtual void update();
	
	/** 親部品を得る */
	inline Container *getParent() { return this->parent; }
	
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
	inline Rect *getRect() { return &this->rect; }
	
	/** 背景色を得る */
	inline unsigned int getBackground() { return this->backColor; }
	
	/** 前景色を得る */
	inline unsigned int getForeground() { return this->foreColor; }
	
	/** フォントスタイルを得る */
	inline int getFontStyle() { return this->fontStyle; }
	
	/** 描画オブジェクトを得る */
	virtual Graphics *getGraphics() { return this->_g; }
	
	/**
	 内部バッファーを得る.
	 VRAMのように直接描画したいときに用いる。
	 Graphicsクラスが持っているような便利なメソッドはない。
	 描画後update()を呼ぶことで実際の描画がされる。
	 */
	virtual Image *getBuffer() { return this->_buffer; }
	
	/** フォント情報を得る */
	inline FontMetrics *getFontMetrics() { return this->_metrics; }
	
	/**
	 メインウィンドウを得る.
	 メインウィンドウとは全ての部品が乗っている一番下のウィンドウ。
	 メインウィンドウは各アプリケーションごとに必ず１つ存在している。
	 */
	virtual Control *getMainWindow();
	
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
	virtual void setRect(int x, int y, int width, int height);
	
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
	virtual void setParent(Container *parent);
	
	/** 背景色を設定する */
	virtual void setBackground(unsigned int backColor);
	
	/** 前景色を設定する */
	virtual void setForeground(unsigned int foreColor);
	
	/**
	 フォントスタイル（通常、太字、斜字、固定幅）を設定する.
	 各スタイルを"|"で組み合わせることも可能。
	 @param style フォントスタイル (Font::PLAIN / Font::BOLD / Font::ITALIC / Font::FIXED )
	*/
	virtual void setFontStyle(int style);
};

#endif // _CONTROL_H_INCLUDED_
