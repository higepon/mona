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
 フォント情報クラス
 */
public class FontMetrics {
	/** フォントオフセットリスト長さ */
	public static int offsetListLength;
	/** フォントオフセットリスト */
	public static int[] offsetList;
	/** フォントデータ */
	public static byte[] defaultFontData;
	/** フォントスタイル */
	private int fontStyle;

	private native void create(byte[] data);

	/** コンストラクタ */
	public FontMetrics() {
		this.fontStyle = Font.PLAIN;
		if (defaultFontData == null) {
			this.defaultFontData = new byte[167921];
			create(this.defaultFontData);
		}
		
		// モナーフォントの構造
		// 3 - 0 フォント数
		// ? - 3 フォント構造体 ( ? = フォント数 )
		// 
		// フォント構造体
		// 3 - 0 UCS-4 (UTF-16)
		//     4 幅 (fw)
		//     5 高さ (fh)
		// ? - 6 ビットパターン ( ? = (fw * fh + 7) / 8 )
		
		offsetListLength = (defaultFontData[1] & 0xFF) << 8 | defaultFontData[0] & 0xFF;
		
		// 文字までのオフセット(byte)
		offsetList = new int[65536];
		
		// オフセットリストを作る
		int pos = 4;
		for (int i = 0; i < offsetListLength; i++) {
			offsetList[(defaultFontData[pos + 1] & 0xFF) << 8 | defaultFontData[pos] & 0xFF] = pos;
			pos += 4;
			int fw = defaultFontData[pos++] & 0xFF;
			int fh = defaultFontData[pos++] & 0xFF;
			pos += (int)((fw * fh + 7) / 8);
		}
	}
	
	/**
	 UCS-4コードを受け取って展開済み文字データを返す
	 @param ucs4 UCS-4コード（4バイト）
	 @param offset [out] 可変フォントでは offset = width、固定フォントでは offset > width
	 @param width [out] 文字の幅
	 @param height [out] 文字の高さ
	 @param data [out] 展開済み文字データ
	*/
	/*public boolean decodeCharacter(wchar ucs4, int* offset, int* width, int* height, char* data) {
		if (ucs4 <= 0xFFFF && defaultFontData != null && offsetList[ucs4] != 0) {
			int fw = defaultFontData[offsetList[ucs4] + 4];
			int fh = defaultFontData[offsetList[ucs4] + 5];
			//System.out.print("fontStyle = %d,", this.fontStyle);
			if ((this.fontStyle & 0x100) == Font.FIXED) {
				if (ucs4 < 128 || 0xff60 < ucs4) {
					*offset = 6;
				} else {
					*offset = 12;
				}
			} else {
				*offset = fw;
			}
			*width  = fw;
			*height = fh;
			memcpy(data, &defaultFontData[offsetList[ucs4] + 6], (int)((fw * fh + 7) / 8));
			return true;
		} else {
			return false;
		}
		return false;
	}*/
	
	/**
	 文字列の幅を得る
	 @param str 文字列（複数行対応）
	 */
	public int getWidth(String str) {
		// nullチェック
		if (str.length() == 0 || this.defaultFontData == null) return 0;
		
		int w = 0;
		for (int i = 0; i < str.length(); i++) {
			char c = str.charAt(i);
			if (c == '\n') {
				break;
			}
			if (c <= 0xFFFF) {
				if ((this.fontStyle & 0x100) == Font.FIXED) {
					if (c < 128 || 0xff60 < c) {
						w += 6;
					} else {
						w += 12;
					}
				} else {
					w += defaultFontData[offsetList[c] + 4];
				}
			}
		}
		
		return w;
	}
	
	/**
	 文字列の高さを得る
	 @param str 文字列（複数行対応）
	 */
	public int getHeight(String str) {
		int h = 12;
		for (int i = 0; i < str.length(); i++) {
			if (str.charAt(i) == '\n') {
				h += 12; // モナーフォントは高さが12ドット
			}
		}
		
		return h;
	}
	
	/** フォントスタイルを得る */
	public int getFontStyle() { return this.fontStyle; }
	
	/** フォントスタイルを設定する */
	public void setFontStyle(int style) { this.fontStyle = style; }
}
