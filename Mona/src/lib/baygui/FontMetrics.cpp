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

// フォントデータとオフセットリストは毎回生成・開放していると遅くなるのでstaticで保持する
int FontMetrics::offsetListLength = 0;
int *FontMetrics::offsetList = NULL;
unsigned char *FontMetrics::defaultFontData = NULL;

FontMetrics::FontMetrics()
{
	this->fontStyle = Font::PLAIN;
	if (defaultFontData != NULL) return;
	
	// フォント取得メッセージを投げる
	MessageInfo info;
	MonAPI::Message::sendReceive(&info, this->guisvrID, MSG_GUISERVER_GETFONT, 0, 0, 0, NULL);
	byte* font_data = MonAPI::MemoryMap::map(info.arg2);
	if (font_data == NULL) {
		printf("%s:%d:ERROR: can not get font!\n", __FILE__, __LINE__);
		exit(1);
	} else {
		defaultFontData = (unsigned char *)malloc(info.arg3);
		memcpy(defaultFontData, font_data, info.arg3);
		MonAPI::MemoryMap::unmap(info.arg2);
		
		// モナーフォントの構造
		// 3 - 0 フォント数
		// ? - 3 フォント構造体 ( ? = フォント数 )
		// 
		// フォント構造体
		// 3 - 0 UCS-4 (UTF-16)
		//     4 幅 (fw)
		//     5 高さ (fh)
		// ? - 6 ビットパターン ( ? = (fw * fh + 7) / 8 )
		
		offsetListLength = inGetUInt16(&defaultFontData[0]);
		
		// 文字までのオフセット(byte)
		offsetList = (int *)malloc(65536);
		memset(offsetList, 0, 65536);
		
		// オフセットリストを作る
		int pos = 4;
		for (int i = 0; i < offsetListLength; i++) {
			offsetList[inGetUInt16(&defaultFontData[pos])] = pos;
			pos += 4;
			int fw = defaultFontData[pos++];
			int fh = defaultFontData[pos++];
			pos += (int)((fw * fh + 7) / 8);
		}
	}
}

FontMetrics::~FontMetrics()
{
}

bool FontMetrics::decodeCharacter(wchar ucs4, int *offset, int *width, int *height, char *data)
{
	if (ucs4 <= 0xFFFF && defaultFontData != NULL && offsetList[ucs4] != 0) {
		int fw = defaultFontData[offsetList[ucs4] + 4];
		int fh = defaultFontData[offsetList[ucs4] + 5];
		//printf("fontStyle = %d,", this->fontStyle);
		if ((this->fontStyle & 0x100) == Font::FIXED) {
			if (ucs4 < 128 || 0xff60 < ucs4) {
				*offset = 8;
			} else {
				*offset = 16;
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
}

int FontMetrics::getWidth(String str)
{
	// NULLチェック
	if (str.length() == 0 || defaultFontData == NULL) return 0;
	
	int w = 0;
	for (int i = 0; i < str.length(); i++) {
		wchar c = str[i];
		if (c == '\n') {
			break;
		}
		if (c <= 0xFFFF) {
			if ((this->fontStyle & 0x100) == Font::FIXED) {
				if (c < 128 || 0xff60 < c) {
					w += 8;
				} else {
					w += 16;
				}
			} else {
				w += defaultFontData[offsetList[c] + 4];
			}
		}
	}
	
	return w;
}

int FontMetrics::getHeight(String str)
{
	int h = 12;
	for (int i = 0; i < str.length(); i++) {
		if (str[i] == '\n') {
			h += 12; // モナーフォントは高さが12ドット
		}
	}
	
	return h;
}
