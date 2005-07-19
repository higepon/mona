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

#if !defined(_GRAPHICS_H_INCLUDED_)
#define _GRAPHICS_H_INCLUDED_

namespace baygui {
	/**
	 描画クラス
	*/
	class Graphics : public Object {
	private:
		/** 描画開始X座標 */
		int tx;
		/** 描画開始Y座標 */
		int ty;
		/** クリッピング領域X座標 */
		int cx;
		/** クリッピング領域Y座標 */
		int cy;
		/** クリッピング領域幅 */
		int cw;
		/** クリッピング領域高さ*/
		int ch;
		/** 色（4バイト） */
		dword rgb24;
		/** フォントスタイル */
		int fontStyle;
		/** 内部バッファー */
		Image* image;

	public:
		/** コンストラクタ */
		Graphics();

		/**
		 コンストラクタ
		 @param image 内部バッファー
		*/
		Graphics(Image* image);
		
		/** デストラクタ */
		virtual ~Graphics();
		
		/**
		 点描画
		 @param x X座標
		 @param y Y座標
		 @param color 描画する色
		 */
		void drawPixel(int x, int y, dword color);
		
		/**
		 イメージ描画
		 @param image イメージ
		 @param x 描画開始X座標
		 @param y 描画開始Y座標
		 */
		void drawImage(Image* image, int x, int y);
		
		/**
		 直線描画
		 @param x0 始点X
		 @param y0 始点Y
		 @param x1 終点X
		 @param y1 終点Y
		 */
		void drawLine(int x0, int y0, int x1, int y1);
		
		/**
		 矩形描画
		 @param x 始点X
		 @param y 始点Y
		 @param width 幅
		 @param height 高さ
		 */
		void drawRect(int x, int y, int width, int height);
		
		/**
		 円描画
		 @param x0 中心X
		 @param y0 中心Y
		 @param r  半径
		 */
		void drawCircle(int x0, int y0, int r);
		
		/**
		 文字列描画
		 @param str 文字列（複数行対応）
		 @param x 始点X
		 @param y 始点Y
		 */
		void drawString(String str, int x, int y);
		
		/**
		 円塗りつぶし描画
		 @param x0 中心X
		 @param y0 中心Y
		 @param r  半径
		 */
		void fillCircle(int x0, int y0, int r);
		
		/**
		 矩形ぬりつぶし描画
		 @param x 始点X
		 @param y 始点Y
		 @param width 幅
		 @param height 高さ
		 */
		void fillRect(int x, int y, int width, int height);
		
		/**
		 座標設定
		 @param x 始点X
		 @param y 始点Y
		 */
		void translate(int x, int y);
		
		/** フォントスタイルを得る */
		inline int getFontStyle() { return this->fontStyle; }
		
		/**
		 クリッピング領域設定
		 @param cx 始点X
		 @param cy 始点Y
		 @param cw 終点X
		 @param ch 終点Y
		 */
		void setClip(int cx, int cy, int cw, int ch);
		
		/**
		 色設定
		 @param r 赤(0-255)
		 @param g 緑(0-255)
		 @param b 青(0-255)
		 */
		void setColor(unsigned char r, unsigned char g, unsigned char b);
		
		/**
		 色設定
		 @param color (0x0-0xFFFFFF)
		 */
		void setColor(dword color);
		
		/**
		 フォントスタイル（通常、太字、斜字、固定幅）を設定する.
		 各スタイルを"|"で組み合わせることも可能。
		 @param style フォントスタイル (Font::PLAIN / Font::BOLD / Font::ITALIC / Font::FIXED )
		*/
		void setFontStyle(int style);
	};
}

#endif // _GRAPHICS_H_INCLUDED_
