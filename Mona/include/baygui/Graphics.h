/*
Copyright (c) 2004 bayside
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. The name of the author may not be used to endorse or promote products
   derived from this software without specific prior written permission.

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

#if !defined(_GRAPHICS_H_INCLUDED_)
#define _GRAPHICS_H_INCLUDED_

/**
 描画クラス
*/
class Graphics : public Object {
private:
	int tx, ty, cx, cy, cw, ch;
	unsigned char r, g, b;
	unsigned int rgb24;
	bool xormode;

	void drawPixelXOR(int x, int y, unsigned int color);

public:
	/** MONA描画用スクリーンクラス */
	MonAPI::Screen *screen;
	Graphics::Graphics();
	virtual Graphics::~Graphics();
	virtual void drawImage(Image *image, int x, int y);
	virtual void drawImage(Image *image, int x, int y, int w, int h);
	virtual void drawPixel(int x, int y, unsigned int color);
	virtual void drawLine(int x0, int y0, int x1, int y1);
	virtual void drawRect(int x, int y, int width, int height);
	virtual void drawText(char *s, int x, int y);
	virtual void fillRect(int x, int y, int width, int height);
	virtual void translate(int x, int y);
	virtual void setClip(int cx, int cy, int cw, int ch);
	virtual void setColor(unsigned char r, unsigned char g, unsigned char b);
	virtual void setXORMode(bool mode);
};

#endif // _GRAPHICS_H_INCLUDED_
