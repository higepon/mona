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
