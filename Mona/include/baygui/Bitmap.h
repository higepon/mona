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

#ifndef __BAYGUI_BITMAP_H__
#define __BAYGUI_BITMAP_H__

namespace baygui
{
	/**
	 ビットマップ.
	 内部的に符号なし32ビットの配列をもっている。
	 GUIサーバー上のビットマップオブジェクトとの仲立ちも行う。
	 */
	class Bitmap : public Object
	{
	protected:
		/** 幅 */
		int width;
		/** 高さ */
		int height;
		/** 内部データ */
		_A<unsigned int> buffer;
		/** GUIサーバー上のビットマップオブジェクト */
		guiserver_bitmap* _object;
	
	public:
		virtual char* className() { return "System.Drawing.Bitmap"; }
		
		Bitmap(int width, int height);
		
		Bitmap(char* fileName);
		
		virtual ~Bitmap();
		
		inline unsigned int getHandle() {
			return this->_object->Handle;
		}
		
		virtual void flush();
		
		inline int getWidth() {
			return this->width;
		}
		
		inline int getHeight() {
			return this->height;
		}
		
		/** 指定した位置の色 (符号なし32ビット) を得る */
		inline unsigned int operator [](int index) { return this->buffer[index]; }
		
		/** 内部バッファーを得る */
		inline unsigned int* get() { return this->buffer.get(); }
		
		/** 色を設定する */
		void setPixel(int x, int y, unsigned int c);
		
		/** 指定した座標の色を得る */
		inline unsigned int getPixel(int x, int y)
		{
			if (x < 0 || this->width <= x || y < 0 || this->height <= y) {
				return DEFAULT_TRANSCOLOR;
			} else {
				return this->buffer[x + y * this->width];
			}
		}
	};
}

#endif  // __BAYGUI_BITMAP_H__
