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

#ifndef __BAYGUI_FONTMETRICS_H__
#define __BAYGUI_FONTMETRICS_H__

namespace baygui
{
	/** フォント情報 */
	class FontMetrics : public Object
	{
	private:
		/** シングルトンインスタンス */
		static FontMetrics *instance;
		/** フォントオフセットリスト長さ */
		int offsetListLength;
		/** フォントオフセットリスト */
		int *offsetList;

	public:
		virtual char* className() { return "baygui.FontMetrics"; }
		
		FontMetrics::FontMetrics();
		
		virtual FontMetrics::~FontMetrics();
		
		/** ucs4コードからフォントのビットデータを得る */
		bool decodeCharacter(wchar ucs4, int *width, int *height, char *data);
		
		/** 文字列の幅を得る */
		int getWidth(String str);
		
		/** 文字列の高さを得る (複数行対応) */
		int getHeight(String str);
	};
}

#endif // __BAYGUI_FONTMETRICS_H__
