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

#include <baygui.h>

FontMetrics *FontMetrics::instance = NULL;

namespace baygui
{
	FontMetrics::FontMetrics()
	{
		offsetListLength = 0;
		offsetList = NULL;
		unsigned char* fp = Application::defaultFontData;
		
		if (fp != NULL)
		{
			// モナーフォントの構造
			// 3 - 0 フォント数
			// ? - 3 フォント構造体 ( ? = フォント数 )
			// 
			// フォント構造体
			// 3 - 0 UCS-4 (UTF-16)
			//     4 幅 (fw)
			//     5 高さ (fh)
			// ? - 6 ビットパターン ( ? = (fw * fh + 7) / 8 )
			
			offsetListLength = GET_UINT16(&fp[0]);
			
			// 文字までのオフセット(byte)
			offsetList = (int *)malloc(65536);
			memset(offsetList, 0, 65536);
			
			// オフセットリストを作る
			int pos = 4;
			for (int i = 0; i < offsetListLength; i++) {
				offsetList[GET_UINT16(&fp[pos])] = pos;
				pos += 4;
				int fw = fp[pos++];
				int fh = fp[pos++];
				pos += (int)((fw * fh + 7) / 8);
			}
		}
	}
	
	FontMetrics::~FontMetrics()
	{
		delete[] offsetList;
	}
	
	bool FontMetrics::decodeCharacter(wchar ucs4, int *width, int *height, char *data)
	{
		unsigned char* fp = Application::defaultFontData;
		
		if (ucs4 <= 0xFFFF && fp != NULL && offsetList[ucs4] != 0) {
			int fw = fp[offsetList[ucs4] + 4];
			int fh = fp[offsetList[ucs4] + 5];
			*width  = fw;
			*height = fh;
			memcpy(data, &fp[offsetList[ucs4] + 6], (int)((fw * fh + 7) / 8));
			return true;
		} else {
			return false;
		}
	}
	
	int FontMetrics::getWidth(String str)
	{
		unsigned char* fp = Application::defaultFontData;
		
		// NULLチェック
		if (str.length() == 0 || fp == NULL) return 0;
		
		int w = 0;
		for (int i = 0; i < str.length(); i++) {
			wchar c = str[i];
			if (c == '\n') {
				break;
			}
			if (c <= 0xFFFF) {
				w += fp[offsetList[c] + 4];
			}
		}
		
		return w;
	}
	
	int FontMetrics::getHeight(String str)
	{
		// NULLチェック
		if (str.length() == 0) return 0;
		
		int h = 12;
		for (int i = 0; i < str.length(); i++) {
			if (str[i] == '\n') {
				h += 12; // モナーフォントは高さが12ドット
			}
		}
		
		return h;
	}
}
