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
Control::Control()
{
	this->enabled = true;
	this->focused = false;
	this->x = this->y = this->height = this->width = this->offsetX = this->offsetY = 0;
	this->fontStyle = FONT_PLAIN;
	this->_buffer = NULL;
	this->_g = NULL;
	this->focusEvent.type = FOCUS_IN;
	this->focusEvent.source = this;
	this->backColor = DEFAULT_BACKCOLOR;
	this->foreColor = DEFAULT_FORECOLOR;
}

/** デストラクタ */
Control::~Control()
{
	dispose();
}

/**
 部品生成時に呼ばれる.
 dispose()後に呼ぶと再初期化できる。
*/
void Control::create()
{
	if (this->_buffer != NULL) return;

	// 内部バッファー、描画オブジェクトの生成
	this->_buffer = new Image(width, height);
	this->_g = new Graphics(this->_buffer);
	this->_metrics = new FontMetrics();
}

/**
 部品破棄時に呼ばれる.
 後にcreate()を呼ぶと再初期化できる。
*/
void Control::dispose()
{
	delete(_buffer);
	delete(_g);
	delete(_metrics);
}

/**
 イベント処理
 @param  [in] event イベントオブジェクト
*/
void Control::onEvent(Event *event)
{
}

/**
 描画処理
 @param [in] g 描画オブジェクト
*/
void Control::onPaint(Graphics *g)
{
}

/**
 イベント処理を依頼する
 @param [in] event イベントオブジェクト
*/
void Control::postEvent(Event *event)
{
	onEvent(event);
	// 親部品にイベントを投げる
	if (getParent() != NULL) {
		getParent()->onEvent(event);
	}
}

/** 再描画 */
void Control::repaint()
{
	if (this->_buffer == NULL) return;
	setFontStyle(this->fontStyle);
	onPaint(this->_g);
	update();
}

/** 領域更新 */
void Control::update()
{
	Control *c = getMainWindow();
	c->getGraphics()->drawImage(this->_buffer, this->x, this->y);
	c->update();
}

/** メインウィンドウを得る */
Control *Control::getMainWindow()
{
	if (this->parent == NULL) {
		return this;
	} else {
		return this->parent->getMainWindow();
	}
}

/**
 活性状態設定
 @param enabled 活性状態 (true / false)
 */
void Control::setEnabled(bool enabled)
{
	this->enabled = enabled;
}

/**
 フォーカス状態を設定する
 @param focused フォーカス状態 (true / false)
 */
void Control::setFocused(bool focused)
{
	if (this->focused == true && focused == false) {
		//syscall_print("FOCUS_OUT,");
		this->focused = focused;
		this->focusEvent.type = FOCUS_OUT;
		postEvent(&this->focusEvent);
	} else if (this->focused == false && focused == true) {
		//syscall_print("FOCUS_IN,");
		this->focused = focused;
		this->focusEvent.type = FOCUS_IN;
		postEvent(&this->focusEvent);
	}
}

/**
 表示状態を設定する
 @param visible 表示状態 (true / false)
 */
void Control::setVisible(bool visible)
{
	this->visible = visible;
}

/**
 大きさを設定する
 @param x x座標
 @param y y座標
 @param width 幅
 @param height 高さ
*/
void Control::setRect(int x, int y, int width, int height)
{
	this->x = x;
	this->y = y;
	this->height = height;
	this->width = width;
	this->rect.x = x;
	this->rect.y = y;
	this->rect.width = width;
	this->rect.height = height;
}

/**
 位置を変更する
 @param x X座標
 @param y Y座標
*/
void Control::setLocation(int x, int y)
{
	if (this->x == x && this->y == y) return;
	
	this->x = x;
	this->y = y;
	this->rect.x = x;
	this->rect.y = y;
}

/**
 親コンポーネントを設定する
 @param parent 親コンポーネント
*/
void Control::setParent(Container *parent)
{
	this->parent = parent;
}

/** 背景色を設定する */
void Control::setBackground(unsigned int backColor)
{
	this->backColor = backColor;
}

/** 前景色を設定する */
void Control::setForeground(unsigned int foreColor)
{
	this->foreColor = foreColor;
}

/**
 フォントスタイル（通常、太字、斜字、太字＆斜字）を設定する
 @param style FONT_PLAIN、FONT_BOLD、FONT_ITALIC、FONT_BOLD | FONT_ITALIC
 */
void Control::setFontStyle(int style)
{
	if (this->_metrics != NULL) {
		_metrics->setFontStyle(style);
	}
	if (this->_g != NULL) {
		_g->setFontStyle(style);
	}
	this->fontStyle = style;
}
