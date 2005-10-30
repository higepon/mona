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

package java.awt;

import java.util.ArrayList;

/**
 チェックボックスグループクラス
*/
public class CheckboxGroup {
	private ArrayList checkboxList;
	
	/** コンストラクタ */
	public CheckboxGroup() {
		this.checkboxList = new ArrayList();
	}
	
	/** チェックボックスを追加する */
	public void add(Checkbox check) {
		// nullチェック
		if (check == null) return;
		check.setCheckboxGroup(this);
		this.checkboxList.add(check);
	}
	
	/** 選択されたチェックボックスを得る */
	public Checkbox getSelectedCheckbox() {
		int I = this.checkboxList.size();
		for (int i = 0; i < I; i++) {
			Checkbox check = (Checkbox)(this.checkboxList.get(i));
			if (check.getChecked() == true) return check;
		}
		return null;
	}
	
	/** イベントハンドラ */
	public void processEvent(AWTEvent event) {
		// 選択されているチェックボックス以外を未選択にする
		int I = this.checkboxList.size();
		for (int i = 0; i < I; i++) {
			Checkbox check = (Checkbox)(this.checkboxList.get(i));
			if (event.getSource() != check) {
				check.setChecked(false);
			}
		}
	}
}
