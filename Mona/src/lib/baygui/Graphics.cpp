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

namespace baygui
{
	Graphics::Graphics(_P<Bitmap> image)
	{
		this->image = image;
	}
	
	//_P<Graphics> Graphics::getGraphics(_P<Bitmap> image)
	//{
	//	return new Graphics(image);
	//}
	
	Graphics::~Graphics()
	{
		this->dispose();
	}
	
	void Graphics::dispose()
	{
		this->image = NULL;
	}
	
	void Graphics::drawPixel(int x, int y, unsigned int c)
	{
		int w = this->clientRect.Width, h = this->clientRect.Height;
		if ((w != 0 || h != 0) && (x >= w || y >= h)) return;
		this->image->setPixel(x + this->clientRect.X, y + this->clientRect.Y, c);
	}
	
	void Graphics::drawLine(int x1, int y1, int x2, int y2)
	{
		int dx = ABS(x1 - x2), dy = ABS(y1 - y2);
		if (dx == 0 && dy == 0) {
			this->drawPixel(x1, y1, this->color);
		} else if (dx >= dy) {
			if (x1 > x2) {
				int tx = x1; x1 = x2; x2 = tx;
				int ty = y1; y1 = y2; y2 = ty;
			}
			for (int x = x1; x <= x2; x++) {
				this->drawPixel(x, ((2 * y1 + 2 * (y2 - y1) * (x - x1) / (x2 - x1)) + 1) / 2, this->color);
			}
		} else {
			if (y1 > y2) {
				int tx = x1; x1 = x2; x2 = tx;
				int ty = y1; y1 = y2; y2 = ty;
			}
			for (int y = y1; y <= y2; y++) {
				this->drawPixel((2 * x1 + 2 * (x2 - x1) * (y - y1) / (y2 - y1) + 1) / 2, y, this->color);
			}
		}
	}
	
	void Graphics::drawRect(int x, int y, int width, int height)
	{
		if (width  < 0) {
			x += width;
			width  = -width;
		}
		if (height < 0) {
			y += height;
			height = -height;
		}
		int xw = x + width - 1, yh = y + height - 1;
		for (int xx = x; xx <= xw; xx++) {
			this->drawPixel(xx, y , this->color);
			this->drawPixel(xx, yh, this->color);
		}
		for (int yy = y + 1; yy <= yh - 1; yy++) {
			this->drawPixel(x , yy, this->color);
			this->drawPixel(xw, yy, this->color);
		}
	}
	
	void Graphics::fillRect(int x, int y, int width, int height)
	{
		if (width  < 0) {
			x += width;
			width  = -width;
		}
		if (height < 0) {
			y += height;
			height = -height;
		}
		int xw = x + width, yh = y + height;
		for (int yy = y; yy < yh; yy++) {
			for (int xx = x; xx < xw; xx++) {
				this->drawPixel(xx, yy, this->color);
			}
		}
	}
	
	void Graphics::drawText(char* str, int x, int y)
	{
		int i, j, k, pos, bit, width, height, w = 0, h = 0;
		_P<FontMetrics> manager = new FontMetrics();
		
		// NULLチェック
		if (str == NULL || strlen(str) == 0) return;
		
		String s = String(str);
		
		for (i = 0; i < s.length(); i++) {
			pos = 0;
			bit = 1;
			char fp[256];
			if (s[i] == '\n') {
				w = 0;
				h += 12;
			}
			if (manager->decodeCharacter(s[i], &width, &height, fp) == true) {
				//unsigned char *fp = list[i]->getData();
				for (j = 0; j < height; j++) {
					for (k = 0; k < width; k++) {
						// 行パディングなし
						if ((fp[pos] & bit) != 0) {
							// 通常書体
							//if (font->getStyle() == FONT_PLAIN) {
								drawPixel(x + w + k, y + h + j, this->color);
							// 太字体
							//} else if (font->getStyle() == FONT_BOLD) {
							//	drawPixel(x + w + k, y + j);
							//	drawPixel(x + w + k + 1, y + j);
							// 斜字体
							//} else if (font->getStyle() == FONT_ITALIC) {
							//	drawPixel(x + w + k + (height - j) / 4, y + j);
							// 太字体＋斜字体
							//} else if (font->getStyle() == FONT_BOLD | FONT_ITALIC) {
							//	drawPixel(x + w + k + (height - j) / 4, y + j);
							//	drawPixel(x + w + k + (height - j) / 4 + 1, y + j);
							//}
						}
						bit <<= 1;
						if (bit == 256) {
							pos++;
							bit = 1;
						}
					}
				}
				w += width;
			}
		}
	}
	
	void Graphics::drawImage(_P<Bitmap> image, int x, int y)
	{
		this->drawImage(image, x, y, Rect(0, 0, image->getWidth(), image->getHeight()));
	}
	
	void Graphics::drawImage(_P<Bitmap> image, int x, int y, Rect r)
	{
		int dw = this->clientRect.Width, dh = this->clientRect.Height;
		int x1 = x, y1 = y, x2 = x + r.Width, y2 = y + r.Height;
		if (x1 < 0) x1 = 0;
		if (y1 < 0) y1 = 0;
		if (x2 > dw) x2 = dw;
		if (y2 > dh) y2 = dh;
		x1 += r.X - x;
		y1 += r.Y - y;
		x2 += r.X - x;
		y2 += r.Y - y;
		if (x1 < 0) x1 = 0;
		if (y1 < 0) y1 = 0;
		if (x2 > image->getWidth ()) x2 = image->getWidth ();
		if (y2 > image->getHeight()) y2 = image->getHeight();
		x1 -= r.X;
		y1 -= r.Y;
		x2 -= r.X;
		y2 -= r.Y;
		for (int yy = y1, y3 = y + y1, y4 = r.Y + y1; yy < y2; yy++, y3++, y4++) {
			for (int xx = x1, x3 = x + x1, x4 = r.X + x1; xx < x2; xx++, x3++, x4++) {
				this->drawPixel(x3, y3, image->getPixel(x4, y4));
			}
		}
	}
}
