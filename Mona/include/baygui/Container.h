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

#if !defined(_CONTAINER_H_INCLUDED_)
#define _CONTAINER_H_INCLUDED_

/**
 コンテナークラス
*/
class Container : public Control {
private:
	/** 部品一覧 */
	LinkedList *controlList;

public:
	/** コンストラクタ */
	Container();
	
	/** デストラクタ */
	virtual ~Container();
	
	/**
	 活性部品を得る
	 @return 活性部品（なければNULL）
	*/
	virtual Control *findChild();
	
	/**
	 部品を得る
	 @param x 指定するX座標
	 @param y 指定するY座標
	 */
	virtual Control *findChild(int x, int y);
	
	/**
	 指定した部品を追加する
	 @param control 指定する部品
	 */
	virtual void add(Control *control);
	
	/**
	 指定した部品を削除する
	 @param control 指定する部品
	 @return 削除された部品（なければNULL）
	 */
	virtual void remove(Control *control);
	
	/** イベント処理 */
	virtual void postEvent(Event *event);
	
	/** 再描画 */
	virtual void repaint();
};

#endif // _CONTAINER_H_INCLUDED_
