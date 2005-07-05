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

#ifndef __FONTMETRICS_H__
#define __FONTMETRICS_H__

/**
 フォント情報クラス
 */
class FontMetrics : public Object
{
private:
	/** フォントオフセットリスト長さ */
	static int offsetListLength;
	/** フォントオフセットリスト */
	static int *offsetList;
	/** フォントデータ */
	static unsigned char *defaultFontData;
	/** フォントスタイル */
	int fontStyle;

public:
	/** コンストラクタ */
	FontMetrics();
	
	/** デストラクタ */
	virtual ~FontMetrics();
	
	/**
	 UCS-4コードを受け取って展開済み文字データを返す
	 @param ucs4 UCS-4コード（4バイト）
	 @param offset [out] 可変フォントでは offset = width、固定フォントでは offset > width
	 @param width [out] 文字の幅
	 @param height [out] 文字の高さ
	 @param data [out] 展開済み文字データ
	*/
	bool decodeCharacter(wchar ucs4, int *offset, int *width, int *height, char *data);
	
	/**
	 文字列の幅を得る
	 @param str 文字列（複数行対応）
	 */
	int getWidth(String str);
	
	/**
	 文字列の高さを得る
	 @param str 文字列（複数行対応）
	 */
	int getHeight(String str);
	
	/** フォントスタイルを得る */
	inline int getFontStyle() { return this->fontStyle; }
	
	/** フォントスタイルを設定する */
	inline void setFontStyle(int style) { this->fontStyle = style; }
};

#endif // __FONTMETRICS_H__
