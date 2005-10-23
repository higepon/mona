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
 描画クラス
 */
public class Graphics {
	/** 描画開始X座標 */
	private int tx;
	/** 描画開始Y座標 */
	private int ty;
	/** クリッピング領域X座標 */
	private int cx;
	/** クリッピング領域Y座標 */
	private int cy;
	/** クリッピング領域幅 */
	private int cw;
	/** クリッピング領域高さ*/
	private int ch;
	/** 色（4バイト） */
	private int rgb24;
	/** フォントスタイル */
	private int fontStyle;
	/** 内部バッファー */
	private Image image;
	
	/** コンストラクタ */
	public Graphics() {
		tx = ty = cx = cy = cw = ch = 0;
		this.rgb24 = 0;
		this.fontStyle = Font.PLAIN;
		this.image = null;
	}

	/**
	 コンストラクタ
	 @param image 内部バッファー
	*/
	public Graphics(Image image) {
		tx = ty = cx = cy = 0;
		this.rgb24 = 0;
		this.fontStyle = Font.PLAIN;
		this.image = image;
		this.cw = image.getWidth();
		this.ch = image.getHeight();
	}

	/**
	 点描画
	 @param x X座標
	 @param y Y座標
	 @param color 描画する色
	 */
	public void drawPixel(int x, int y, int color) {
		this.image.setPixel(tx + x, ty + y, color);
	}

	/**
	 イメージ描画
	 @param image イメージ
	 @param x 描画開始X座標
	 @param y 描画開始Y座標
	 */
	public void drawImage(Image image, int x, int y) {
		for (int i = 0; i < image.getHeight(); i++) {
			for (int j = 0; j < image.getWidth(); j++) {
				drawPixel(j + x, i + y, image.getPixel(j, i));
			}
		}
	}

	/**
	 直線描画
	 @param x1 始点X
	 @param y1 始点Y
	 @param x2 終点X
	 @param y2 終点Y
	 */
	public void drawLine(int x1, int y1, int x2, int y2) {
		int dx = (x1 > x2) ? (x1 - x2) : (x2 - x1);
		int dy = (y1 > y2) ? (y1 - y2) : (y2 - y1);
		
		if (dx == 0 && dy == 0) {
			this.drawPixel(x1, y1, this.rgb24);
		} else if (dx >= dy) {
			if (x1 > x2) {
				int tx = x1; x1 = x2; x2 = tx;
				int ty = y1; y1 = y2; y2 = ty;
			}
			for (int x = x1; x <= x2; x++) {
				this.drawPixel(x, ((2 * y1 + 2 * (y2 - y1) * (x - x1) / (x2 - x1)) + 1) / 2, this.rgb24);
			}
		} else {
			if (y1 > y2) {
				int tx = x1; x1 = x2; x2 = tx;
				int ty = y1; y1 = y2; y2 = ty;
			}
			for (int y = y1; y <= y2; y++) {
				this.drawPixel((2 * x1 + 2 * (x2 - x1) * (y - y1) / (y2 - y1) + 1) / 2, y, this.rgb24);
			}
		}
	}

	/**
	 矩形描画
	 @param x 始点X
	 @param y 始点Y
	 @param width 幅
	 @param height 高さ
	 */
	public void drawRect(int x, int y, int width, int height) {
		if (width < 0) {
			x += width;
			width = -width;
		}
		if (height < 0) {
			y += height;
			height = -height;
		}
		int xw = x + width - 1, yh = y + height - 1;
		for (int xx = x; xx <= xw; xx++) {
			this.drawPixel(xx, y , this.rgb24);
			this.drawPixel(xx, yh, this.rgb24);
		}
		for (int yy = y + 1; yy <= yh - 1; yy++) {
			this.drawPixel(x , yy, this.rgb24);
			this.drawPixel(xw, yy, this.rgb24);
		}
	}

	/**
	 円描画
	 @param x0 中心X
	 @param y0 中心Y
	 @param r  半径
	 */
	public void drawCircle(int x0, int y0, int r) {
		int x, y, f;
		x = r;
		y = 0;
		f = -2 * r + 3;

		while (x >= y) {
			drawPixel(x0 + x, y0 + y, this.rgb24);
			drawPixel(x0 - x, y0 + y, this.rgb24);
			drawPixel(x0 + y, y0 + x, this.rgb24);
			drawPixel(x0 - y, y0 + x, this.rgb24);
			drawPixel(x0 - x, y0 - y, this.rgb24);
			drawPixel(x0 - y, y0 - x, this.rgb24);
			drawPixel(x0 + y, y0 - x, this.rgb24);
			drawPixel(x0 + x, y0 - y, this.rgb24);
			if (f >= 0) {
				x--;
				f -= 4 * x;
			}
			y ++;
			f += 4 * y + 2;
		}
	}

	/**
	 文字列描画
	 @param str 文字列（複数行対応）
	 @param x 始点X
	 @param y 始点Y
	 */
	public void drawString(String str, int x, int y) {
		int pos, bit, offset, width, height, w = 0, h = 0;
		byte[] fp = new byte[256];
		
		// nullチェック
		if (str.length() == 0) return;
		
		FontMetrics fm = new FontMetrics();
		//metrics.setFontStyle(getFontStyle());
		int I = str.length();
		for (int i = 0; i < I; i++) {
			pos = 0;
			bit = 1;
			char ucs4 = str.charAt(i);
			// 改行
			if (ucs4 == '\n') {
				w = 0;
				h += 12;
			}
			if (fm.defaultFontData != null && fm.offsetList[ucs4] != 0) {
				int fw = fm.defaultFontData[fm.offsetList[ucs4] + 4];
				int fh = fm.defaultFontData[fm.offsetList[ucs4] + 5];
				//System.out.print("fontStyle = %d,", fm.getFontStyle());
				if ((fm.getFontStyle() & 0x100) == Font.FIXED) {
					if (ucs4 < 128 || 0xff60 < ucs4) {
						offset = 6;
					} else {
						offset = 12;
					}
				} else {
					offset = fw;
				}
				width  = fw;
				height = fh;
				//memcpy(data, &defaultFontData[offsetList[ucs4] + 6], (int)((fw * fh + 7) / 8));
				// fp にフォントデータを展開する
				int J = (fw * fh + 7) / 8;
				for (int j = 0; j < J; j++) {
					fp[j] = fm.defaultFontData[fm.offsetList[ucs4] + 6 + j];
				}
				// フォントを実際に描画する
				for (int j = 0; j < height; j++) {
					for (int k = 0; k < width; k++) {
						int x0 = x + w + k + (offset - width) / 2;
						// 行パディングなし
						if ((fp[pos] & bit) != 0) {
							// 通常書体
							if ((getFontStyle() & 0x011) == Font.PLAIN) {
								drawPixel(x0, y + h + j, this.rgb24);
							// 太字体
							} else if ((getFontStyle() & 0x011) == Font.BOLD) {
								drawPixel(x0, y + j, this.rgb24);
								drawPixel(x0 + 1, y + j, this.rgb24);
							// 斜字体
							} else if ((getFontStyle() & 0x011) == Font.ITALIC) {
								drawPixel(x0 + (height - j) / 4, y + j, this.rgb24);
							// 太字体＋斜字体
							} else if ((getFontStyle() & 0x011) == (Font.BOLD | Font.ITALIC)) {
								drawPixel(x0 + (height - j) / 4, y + j, this.rgb24);
								drawPixel(x0 + (height - j) / 4 + 1, y + j, this.rgb24);
							}
						}
						bit <<= 1;
						if (bit == 256) {
							pos++;
							bit = 1;
						}
					}
				}
				w += offset;
			}
		}
	}

	/**
	 円塗りつぶし描画
	 @param x0 中心X
	 @param y0 中心Y
	 @param r  半径
	 */
	public void fillCircle(int x0, int y0, int r) {
		int i;
		int x, y;
		x = r;
		y = 0;
		int f = -2 * r + 3;

		while(x >= y) {
			for (i = x0 - x; i <= x0 + x; i ++) {
				drawPixel(i, y0 + y, this.rgb24);
				drawPixel(i, y0 - y, this.rgb24);
			}
			for (i = x0 - y; i <= x0 + y; i ++) {
				drawPixel(i, y0 - x, this.rgb24);
				drawPixel(i, y0 + x, this.rgb24);
			}
			if (f >= 0) {
				x--;
				f -= 4 * x;
			}
			y ++;
			f += 4 * y + 2;
		}
	}

	/**
	 矩形ぬりつぶし描画
	 @param x 始点X
	 @param y 始点Y
	 @param width 幅
	 @param height 高さ
	 */
	public void fillRect(int x, int y, int width, int height) {
		if (width < 0) {
			x += width;
			width = -width;
		}
		if (height < 0) {
			y += height;
			height = -height;
		}
		
		int xw = x + width, yh = y + height;
		
		for (int yy = y; yy < yh; yy++) {
			for (int xx = x; xx < xw; xx++) {
				this.drawPixel(xx, yy, this.rgb24);
			}
		}
	}

	/**
	 座標設定
	 @param x 始点X
	 @param y 始点Y
	 */
	public void translate(int x, int y) {
		tx = x;
		ty = y;
	}

	/** フォントスタイルを得る */
	public int getFontStyle() { return this.fontStyle; }

	/**
	 クリッピング領域設定
	 @param cx 始点X
	 @param cy 始点Y
	 @param cw 終点X
	 @param ch 終点Y
	 */
	public void setClip(int cx, int cy, int cw, int ch) {
		this.cx = cx;
		this.cy = cy;
		this.cw = cw;
		this.ch = ch;
	}

	/**
	 色設定
	 @param r 赤(0-255)
	 @param g 緑(0-255)
	 @param b 青(0-255)
	 */
	public void setColor(int r, int g, int b) {
		this.rgb24 = 0xff000000 | (r << 16) | (g << 8) | b;
	}

	/**
	 色設定
	 @param color (0x0-0xffffffff)
	 */
	public void setColor(int color) {
		this.rgb24 = color;
	}

	/**
	 フォントスタイル（通常、太字、斜字、固定幅）を設定する.
	 各スタイルを"|"で組み合わせることも可能。
	 @param style フォントスタイル (Font.PLAIN / Font.BOLD / Font.ITALIC / Font.FIXED )
	*/
	public void setFontStyle(int style) {
		this.fontStyle = style;
	}
}
