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

#if !defined(_CHECKBOX_H_INCLUDED_)
#define _CHECKBOX_H_INCLUDED_

namespace baygui {
	class CheckboxGroup;

	/**
	 チェックボックスクラス
	*/
	class Checkbox : public Component {
	private:
		/** チェックされたかどうか */
		bool checked;
		/** ボタンのラベル */
		String label;
		/** 選択イベント */
		Event itemEvent;
		/** チェックボックスグループ */
		CheckboxGroup* group;

	public:
		/** デフォルトコンストラクタ */
		Checkbox();
		
		/**
		 コンストラクタ
		 @param label ラベル
		 */
		Checkbox(const String& label);
		
		/** デストラクタ */
		virtual ~Checkbox();
		
		/**
		 チェックされたかどうかを設定する
		 @param checked フラグ (true / false)
		 */
		virtual void setChecked(bool checked);
		
		/** チェックボックスグループを設定する */
		inline void setCheckboxGroup(CheckboxGroup* group) { this->group = group; }
		
		/**
		 ラベルを設定する
		 @param label ラベル
		 */
		virtual void setLabel(const String& label);
		
		/** チェックされたかどうかを得る */
		inline bool getChecked() { return this->checked; }
		
		/** チェックボックスグループを得る */
		inline CheckboxGroup* getCheckboxGroup() { return this->group; }
		
		/** ラベルを得る */
		inline char* getLabel() { return this->label.getBytes(); }
		
		/** 描画ハンドラ */
		virtual void paint(Graphics* g);
		
		/** イベントハンドラ */
		virtual void processEvent(Event* event);
	};
}

#endif // _CHECKBOX_H_INCLUDED_
