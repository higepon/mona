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
Control::Control() {
	enabled = true;
	focused = iconified = firstpaint = false;
	x = y = height = width = 0;
	_rect = new Rect(0,0,0,0);
	_g = new Graphics();
	_focusEvent = new Event(FOCUS_IN, this);
	_iconEvent = new Event(ICONIFIED, this);
	backColor = 0xC8C8C8;
	foreColor = 0x000000;
	font = new Font();
	font->setName(FONT_NAME);
	font->setWidth(6);
	font->setHeight(12);
	font->setStyle(FONT_PLAIN);
}

/** デストラクタ */
Control::~Control() {
	delete(_rect);
	delete(_g);
	delete(_focusEvent);
	delete(_iconEvent);
	delete(font);
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
	onPaint(_g);
}

/**
 活性状態を得る
 @return 活性状態 (true / false)
 */
bool Control::getEnabled()
{
	return enabled;
}

/**
 フォーカス状態を得る
 @return フォーカス状態 (true / false)
 */
bool Control::getFocused()
{
	return focused;
}

/**
 アイコン状態を得る
 @return アイコン状態 (true / false)
 */
bool Control::getIconified()
{
	return iconified;
}

/**
 大きさを得る
 @return 大きさ
*/
Rect *Control::getRect()
{
	_rect->x = this->x;
	_rect->y = this->y;
	_rect->height = this->height;
	_rect->width = this->width;
	return _rect;
}

/**
 描画領域を得る
 */
Graphics *Control::getGraphics()
{
	return _g;
}

/**
 親コンポーネントを得る
 @return 親コンポーネント
*/
Container *Control::getParent()
{
	return parent;
}

/** 背景色を設定する */
unsigned int Control::getBackground()
{
	return backColor;
}

/** 前景色を設定する */
unsigned int Control::getForeground()
{
	return foreColor;
}

/** フォントを得る */
Font *Control::getFont()
{
	return font;
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
	this->focused = focused;
	if (focused == false) {
		_focusEvent->type = FOCUS_OUT;
		postEvent(_focusEvent);
	} else {
		_focusEvent->type = FOCUS_IN;
		postEvent(_focusEvent);
	}
}

/**
 アイコン状態を設定する
 @param iconified アイコン状態 (true / false)
 */
void Control::setIconified(bool iconified)
{
	if (iconified == false) {
		this->iconified = iconified;
		_iconEvent->type = DEICONIFIED;
		postEvent(_iconEvent);
	} else {
		this->iconified = iconified;
		_iconEvent->type = ICONIFIED;
		postEvent(_iconEvent);
	}
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
	_g->translate(x,y);
	_g->setClip(x, y, width, height);
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
 フォントを設定する.
 【注意】現状は通常文字か太字かを指定できる程度です。
 */
void Control::setFont(Font *font)
{
	this->font->setName(font->getName());
	this->font->setWidth(font->getWidth());
	this->font->setHeight(font->getHeight());
	this->font->setStyle(font->getStyle());
	_g->setFont(font);
}
