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
Container::Container()
{
	enabled = true;
	firstpaint = false;
	__g = new Graphics();
	controlListPtr = 0;
}

/** デストラクタ */
Container::~Container() {
	delete(__g);
}

/**
 活性部品を得る
 @return 活性部品（なければNULL）
*/
Control *Container::getActiveControl()
{
	int i;
	
	for (i = 0; i < controlListPtr; i++) {
		if (controlList[i]->getEnabled() == true) {
			return controlList[i];
		}
	}
	return NULL;
}

/**
 活性部品を得る
 @param x 指定するX座標
 @param y 指定するY座標
 */
Control *Container::getActiveControl(int x, int y)
{
	int i;
	
	for (i = 0; i < controlListPtr; i++) {
		Rect *rect = controlList[i]->getRect();
		// マウスカーソルがある範囲にウィンドウがあるかどうかチェック
		if (rect->x <= x && x <= rect->x + rect->width && 
			rect->y <= y && y <= rect->y + rect->height)
		{
			return controlList[i];
		}
	}
	return NULL;
}

/**
 指定した部品を活性にする
 @param control 指定する部品
*/
void Container::setActiveControl(Control *control)
{
	int i;
	
	for (i = 0; i < controlListPtr; i++) {
		if (controlList[i] == control) {
			controlList[i]->setEnabled(true);
		} else {
			controlList[i]->setEnabled(false);
		}
	}
}

/**
 指定した部品を追加する
 @param control 指定する部品
 */
void Container::add(Control *control)
{
	// 最後に追加する
	if (controlListPtr < MAX_CONTROLLIST_LEN - 1) {
		// 部品追加
		controlList[controlListPtr] = control;
		controlList[controlListPtr]->setParent(this);
		// 部品をウィンドウの中に配置する
		Rect *rect = controlList[controlListPtr]->getRect();
		controlList[controlListPtr]->setRect
			(x + INSETS_LEFT + rect->x, y + INSETS_TOP + rect->y, rect->width, rect->height);
		// 部品を再描画
		if (firstpaint == true) {
			controlList[controlListPtr]->repaint();
		}
		controlListPtr++;
	}
}

/**
 指定した部品を削除する.
 用がすんだら、返ってきたインスタンスを必ずdeleteすること。
 @param control 指定する部品
 @return 削除された部品（なければNULL）
 */
Control *Container::remove(Control *control)
{
	int i, j;
	
	for (i = 0; i < controlListPtr; i++) {
		if (controlList[i] == control) {
			// 後ろから詰める
			for (j = i; j < controlListPtr - 1; j++) {
				controlList[j] = controlList[j + 1];
			}
			controlListPtr--;
			// 親を再描画
			if (firstpaint == true) {
				repaint();
			}
			return control;
		}
	}
	
	return NULL;
}
