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

package java.util;

/**
 キーと値をセットで持つリストクラス
*/
public class Hashtable {
	/** キーリスト */
	private Vector keyList;
	/** 値リスト */
	private Vector valueList;

	/** デフォルトコンストラクタ */
	public Hashtable() {
		keyList = new Vector();
		valueList = new Vector();
	}

	/** 中身を空にする */
	public void clear() {
		keyList.clear();
		valueList.clear();
	}

	/** キーに相当する値を得る */
	public Object get(Object key) {
		return (key == null) ? null : valueList.get(keyList.indexOf(key));
	}

	/** キーと値の組を追加する */
	public void put(Object key, Object value) {
		if (key == null || value == null) return;
		int index = keyList.indexOf(key);
		if (index >= 0) {
			keyList.remove(index);
			valueList.remove(index);
		}
		keyList.add(key);
		valueList.add(value);
	}

	/** キーに相当する値を削除する */
	public void remove(Object key)
	{
		if (key == null) return;
		int index = keyList.indexOf(key);
		keyList.remove(index);
		valueList.remove(index);
	}
}
