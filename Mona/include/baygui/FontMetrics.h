/*
Copyright (c) 2004 Tino, bayside
All rights reserved.

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
