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

#ifndef __BAYGUI_GRAPHICS_H__
#define __BAYGUI_GRAPHICS_H__

namespace baygui
{
	/**
	 描画クラス.
	 描画はバッファーに対して行われ、画面には反映されない。
	*/
	class Graphics : public Object
	{
	private:
		/** 内部バッファー */
		_P<Bitmap> image;
		/** 色 */
		unsigned int color;
		/** 内部描画領域 */
		Rect clientRect;
	
	public:
		virtual char* className() { return "baygui.Graphics"; }
		
		Graphics(_P<Bitmap> image);
		
		virtual ~Graphics();
		
		void dispose();
		
		/** 点を打つ */
		void drawPixel(int x, int y, unsigned int c);
		
		/** 直線を引く */
		void drawLine(int x1, int y1, int x2, int y2);
		
		/** 矩形を描画する */
		void drawRect(int x, int y, int width, int height);
		
		/** 矩形を塗りつぶす */
		void fillRect(int x, int y, int width, int height);
		
		/** 文字列を描画する */
		void drawText(char* str, int x, int y);
		
		/** イメージを描画する */
		void drawImage(_P<Bitmap> image, int x, int y);
		
		/** イメージを描画する */
		void drawImage(_P<Bitmap> image, int x, int y, Rect r);
		
		/** 色を設定する */
		inline void setColor(unsigned int c) { this->color = c; }
		
		/** 色を設定する */
		inline void setColor(unsigned char r, unsigned char g, unsigned char b) {
			this->color = (0xff000000 | r << 16 | g << 8 | b);
		}
		
		/** オフセットを得る */
		inline Point getOffset() { return this->clientRect.get_Location(); }
		
		/** 内部領域を得る */
		inline void setClientRect(Rect r) { this->clientRect = r; }
	};
}

#endif  // __BAYGUI_GRAPHICS_H__
