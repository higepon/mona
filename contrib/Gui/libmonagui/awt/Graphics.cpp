/*
  Copyright (c) 2005 bayside
                2011 Higepon : Refactoring and added testability.
                               Added proper folding strings.
                2012 Higepon : Added clipping support

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

#include "monagui.h"

namespace monagui {

Graphics::Graphics(Image* image) :
    x_min_(0),
    x_max_(image->getWidth()),
    y_min_(0),
    y_max_(image->getHeight()),
    rgb24_(0),
    fontStyle_(Font::PLAIN),
    image_(image) {
}

Graphics::~Graphics() {
}

void Graphics::drawImage(Image* image, int x, int y) {
  image_->copy(image, x, y);
}

void Graphics::drawLineDirect(int x1, int y1, int x2, int y2) {
  int dx = abs(x1 - x2);
  int dy = abs(y1 - y2);

  if (dx == 0 && dy == 0) {
    drawPixelDirect(x1, y1, rgb24_);
  } else if (dx >= dy) {
    if (x1 > x2) {
      std::swap(x1, x2);
      std::swap(y1, y2);
    }
    for (int x = x1; x <= x2; x++) {
      drawPixelDirect(x, ((2 * y1 + 2 * (y2 - y1) * (x - x1) / (x2 - x1)) + 1) / 2, rgb24_);
    }
  } else {
    if (y1 > y2) {
      std::swap(x1, x2);
      std::swap(y1, y2);
    }
    for (int y = y1; y <= y2; y++) {
      drawPixelDirect((2 * x1 + 2 * (x2 - x1) * (y - y1) / (y2 - y1) + 1) / 2, y, rgb24_);
    }
  }
}

void Graphics::drawRect(int x, int y, int width, int height)
{
  MONA_ASSERT(width >= 0 && height >= 0);
  int xw = x + width - 1, yh = y + height - 1;
  for (int xx = x; xx <= xw; xx++) {
    drawPixel(xx, y , rgb24_);
    drawPixel(xx, yh, rgb24_);
  }
  for (int yy = y + 1; yy <= yh - 1; yy++) {
    drawPixel(x , yy, rgb24_);
    drawPixel(xw, yy, rgb24_);
  }
}

void Graphics::drawClip()
{
  int width = x_max_ - x_min_;
  int height = y_max_ - y_min_;
  int x = x_min_;
  int y = y_min_;

  MONA_ASSERT(width >= 0 && height >= 0);
  int xw = x + width - 1, yh = y + height - 1;
  for (int xx = x; xx <= xw; xx++) {
    drawPixel(xx, y , rgb24_);
    drawPixel(xx, yh, rgb24_);
  }
  for (int yy = y + 1; yy <= yh - 1; yy++) {
    drawPixel(x , yy, rgb24_);
    drawPixel(xw, yy, rgb24_);
  }
}


void Graphics::drawString(String* str, int x, int y)
{
  drawString(str->getBytes(), x, y);
}

int Graphics::drawStringInternal(const String& text, int x, int y, int maxWidth, bool draws)
{
  FontMetrics metrics;
  metrics.setFontStyle(getFontStyle());
  const int FONT_HEIGHT = metrics.getHeight("W");

  if (text.length() == 0) {
    return FONT_HEIGHT;
  }

  int w = 0;
  int h = 0;
  int len = text.length();
  for (int i = 0; i < len; i++) {
    int pos = 0;
    int bit = 1;
    char fp[256];
    // 改行
    if (text.charAt(i) == '\n' || w >= maxWidth) {
      w = 0;
      h += FONT_HEIGHT;
      continue;
    }
    int offset = 0;
    int width = 0;
    int height = 0;
    if (metrics.decodeCharacter(text.charAt(i), &offset, &width, &height, fp)) {
      if (w + offset > maxWidth) {
        w = 0;
        h += FONT_HEIGHT;
      }
      for (int j = 0; j < height; j++) {
        for (int k = 0; k < width; k++) {
          if (draws) {
            int x0 = x + w + k + (offset - width) / 2;
            // 行パディングなし
            if ((fp[pos] & bit) != 0) {
              // 通常書体
              if ((getFontStyle() & 0x011) == Font::PLAIN) {
                drawPixel(x0, y + h + j, rgb24_);
                // 太字体
              } else if ((getFontStyle() & 0x011) == Font::BOLD) {
                drawPixel(x0, y + j, rgb24_);
                drawPixel(x0 + 1, y + j, rgb24_);
                // 斜字体
              } else if ((getFontStyle() & 0x011) == Font::ITALIC) {
                drawPixel(x0 + (height - j) / 4, y + j, rgb24_);
                // 太字体＋斜字体
              } else if ((getFontStyle() & 0x011) == (Font::BOLD | Font::ITALIC)) {
                drawPixel(x0 + (height - j) / 4, y + j, rgb24_);
                drawPixel(x0 + (height - j) / 4 + 1, y + j, rgb24_);
              }
            }
          }
          bit <<= 1;
          if (bit == 256) {
            pos++;
            bit = 1;
          }
        }
      }
      w += offset;
    }
  }
  return h + FONT_HEIGHT;
}

int Graphics::getHeightByString(const String& str, int x, int y, int maxWidth)
{
  const bool draws = false;
  return drawStringInternal(str, x, y, maxWidth, draws);
}

void Graphics::drawString(const String& str, int x, int y, int maxWidth)
{
  drawStringInternal(str, x, y, maxWidth, true);
}

void Graphics::drawString(const String& str, int x, int y)
{
  drawString(str, x, y, x_max_);
}

void Graphics::invartRect(int x, int y, int width, int height)
{
  if (width < 0) {
    x += width;
    width = -width;
  }
  if (height < 0) {
    y += height;
    height = -height;
  }

  int xw = x + width, yh = y + height;

  for (int yy = y; yy < yh; yy++) {
    for (int xx = x; xx < xw; xx++) {
      dword c = image_->getPixel(xx, yy);
      c = (0xff000000) | (~c);
      drawPixel(xx, yy, c);
    }
  }
}

void Graphics::setColor(unsigned char r, unsigned char g, unsigned char b)
{
  rgb24_ = 0xff000000 | (r << 16) | (g << 8) | b;
}

void Graphics::setColor(dword color)
{
  rgb24_ = color;
}

void Graphics::setFontStyle(int style)
{
  fontStyle_ = style;
}
}
