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

/**
 フレームクラス
*/
public class Frame extends Window {
	/** 閉じるボタン (パレット) */
	private int close_palette[] = {
		0xff1c1a1c,
		0xff8c8e8c,
		0xffcccecc,
		0xffacaeac,
		0xffeceeec,
		0xff9c9e9c,
		0xffdcdedc,
		0xffbcbebc,
		0xfffcfefc,
	};

	/** 閉じるボタン (データ) */
	private byte close_data[] = {
		0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x2,
		0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x8,
		0x1,0x0,0x8,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x0,0x8,
		0x1,0x0,0x2,0x5,0x5,0x3,0x3,0x7,0x7,0x2,0x1,0x0,0x8,
		0x1,0x0,0x2,0x5,0x3,0x3,0x7,0x7,0x2,0x2,0x1,0x0,0x8,
		0x1,0x0,0x2,0x3,0x3,0x7,0x7,0x2,0x2,0x6,0x1,0x0,0x8,
		0x1,0x0,0x2,0x3,0x7,0x7,0x2,0x2,0x6,0x6,0x1,0x0,0x8,
		0x1,0x0,0x2,0x7,0x7,0x2,0x2,0x6,0x6,0x4,0x1,0x0,0x8,
		0x1,0x0,0x2,0x7,0x2,0x2,0x6,0x6,0x4,0x4,0x1,0x0,0x8,
		0x1,0x0,0x2,0x2,0x2,0x6,0x6,0x4,0x4,0x8,0x1,0x0,0x8,
		0x1,0x0,0x2,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x0,0x8,
		0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x8,
		0x2,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,
	};
	
	/** タイトル */
	private String title;
	
	/** 閉じるボタンがクリックされたかどうか */
	public boolean getCloseButtonClicked(int px, int py) {
		return (4 <= px && px < 17 && 4 <= py && py < 17) ? true : false;
	}
	
	/** タイトルバーがクリックされたかどうか */
	public boolean getTitlebarClicked(int px, int py) {
		return (0 <= px && px < getWidth() && 0 <= py && py < getInsets().top) ? true : false;
	}

	/** デフォルトコンストラクタ */
	public Frame() {
		/* デフォルトタイトル */
		this.title = "Frame";
		
		/* フチあり */
		this.border = 0;
		
		/* 配置位置 */
		this.insets.top = 22;
		this.insets.bottom = 6;
		this.insets.left = 6;
		this.insets.right = 6;
	}
	
	/**
	 * コピーコンストラクタ
	 * @param title タイトル
	*/
	public Frame(String title) {
		/* デフォルトタイトル */
		this.title = title;
		
		/* フチあり */
		this.border = 0;
		
		/* 配置位置 */
		this.insets.top = 22;
		this.insets.bottom = 6;
		this.insets.left = 6;
		this.insets.right = 6;
	}
	
	/** タイトルを得る */
	public String getTitle() { return this.title; }
	
	/**
	 タイトル設定
	 @param title タイトル
	 */
	public void setTitle(String title) {
		this.title = title;
	}
	
	/** 再描画 */
	public void repaint() {
		if (this.__g == null) return;
		
		/* 矩形ウィンドウ */
		int w = getWidth();
		int h = getHeight();
		
		/* 外枠 */
		__g.setColor(Color.lightGray);
		__g.fillRect(0, 0, w, h);
		__g.setColor(Color.black);
		__g.drawRect(0, 0, w, h);
		
		/* 内枠 */
		__g.setColor(Color.black);
		__g.drawRect(5, 21, w - 10, h - 26);
		
		/* 枠 */
		__g.setColor(Color.white);
		__g.drawLine(1, 1, w - 2, 1);
		__g.drawLine(1, 1, 1, h - 2);
		__g.drawLine(w - 5, 21, w - 5, h - 5);
		__g.drawLine(5, h - 5, w - 5, h - 5);
		__g.setColor(Color.gray);
		__g.drawLine(w - 2, 2, w - 2, h - 2);
		__g.drawLine(2, h - 2, w - 2, h - 2);
		__g.drawLine(4, 20, w - 6, 20);
		__g.drawLine(4, 20, 4, h - 6);
		
		if (getFocused() == true) {
			/* タイトルバー */
			for (int i = 4; i <= 14; i = i + 2) {
				__g.setColor(Color.gray);
				__g.drawLine(20, i, w - 7, i);
				__g.setColor(Color.white);
				__g.drawLine(21, i + 1, w - 6, i + 1);
			}
			
			/* 閉じるボタン */
			for (int i = 0; i < 13; i++) {
				for (int j = 0; j < 13; j++) {
					__g.drawPixel(j + 4, i + 4, close_palette[close_data[i * 13 + j] & 0xFF]);
				}
			}
		}

		/* タイトル */
		int fw = getFontMetrics().getWidth(getTitle());
		int fh = getFontMetrics().getHeight(getTitle());
		__g.setColor(Color.lightGray);
		__g.fillRect(((w - fw) / 2) - 4, 2, fw + 8, getInsets().top - 4);
		if (getFocused() == true) {
			__g.setColor(Color.black);
		} else {
			__g.setColor(Color.gray);
		}
		__g.drawString(getTitle(), ((w - fw) / 2), ((getInsets().top - fh) / 2));
		
		super.repaint();
	}
}
