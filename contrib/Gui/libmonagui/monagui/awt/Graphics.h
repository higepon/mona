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

#if !defined(_GRAPHICS_H_INCLUDED_)
#define _GRAPHICS_H_INCLUDED_

namespace monagui {

class Graphics : public Object {
 public:

  Graphics(Image* image);
  virtual ~Graphics();

  void drawPixelDirect(int x, int y, dword color)
  {
    image_->setPixel(x, y, color);
  }

  int getFontStyle() const {
    return fontStyle_;
  }

  uint32_t getColor() const {
    return rgb24_;
  }

  void drawImage(Image* image, int x, int y);
  void drawRect(int x, int y, int width, int height);
  void drawString(String* str, int x, int y);
  void drawString(const String& str, int x, int y);
  void drawString(const String& str, int x, int y, int maxWidth);
  int getHeightByString(const String& str, int x, int y, int maxWidth);
  void invartRect(int x, int y, int width, int height);
  void setColor(unsigned char r, unsigned char g, unsigned char b);
  void setColor(dword color);
  void setFontStyle(int style);
  void drawClip();


  void setClip(int x, int y, int w, int h) {
    x_min_ = x;
    x_max_ = x + w;
    y_min_ = y;
    y_max_ = y + h;
  }

  void drawLine(int x1, int y1, int x2, int y2) {
    cohenSutherlandLineClip(x1, y1, x2, y2);
  }

  void fillRect(int x, int y, int w, int h) {
    int x_to = x + w;
    int y_to = y + h;
    for (int j = y; j < y_to; j++) {
      drawLine(x, j, x_to - 1, j);
    }
  }

  void drawPixel(int x, int y, uint32_t c) {
    if (x >= x_min_ && x <= x_max_ && y >= y_min_ && y <= y_max_) {
      drawPixelDirect(x, y, c);
    }
  }

 private:
  typedef int OutCode;
  enum {
    INSIDE = 0,  // 0000
    LEFT = 1,    // 0001
    RIGHT = 2,   // 0010
    BOTTOM = 4,  // 0100
    TOP = 8      // 1000
  };

  OutCode ComputeOutCode(double x, double y) {
    OutCode code;
    code = INSIDE;
    if (x < x_min_) {
      code |= LEFT;
    } else if (x > x_max_) {
      code |= RIGHT;
    }
    if (y < y_min_) {
      code |= BOTTOM;
    } else if (y > y_max_) {
      code |= TOP;
    }
    return code;
  }

  // http://en.wikipedia.org/wiki/Cohen%E2%80%93Sutherland
  void cohenSutherlandLineClip(double x0, double y0, double x1, double y1) {
    OutCode outcode0 = ComputeOutCode(x0, y0);
    OutCode outcode1 = ComputeOutCode(x1, y1);
    bool accept = false;
    while (true) {
      if (!(outcode0 | outcode1)) {
        // Bitwise OR is 0. Trivially accept and get out of loop
        accept = true;
        break;

      } else if (outcode0 & outcode1) {
        // Bitwise AND is not 0. Trivially reject and get out of loop
        break;
      } else {
        // failed both tests, so calculate the line segment to clip
        // from an outside point to an intersection with clip edge
        double x = 0.0;
        double y = 0.0;
        // At least one endpoint is outside the clip rectangle; pick it.
        OutCode outcodeOut = outcode0? outcode0 : outcode1;
        // Now find the intersection point;
        // use formulas y = y0 + slope * (x - x0),
        //              x = x0 + (1 / slope) * (y - y0)
        if (outcodeOut & TOP) {
          // point is above the clip rectangle
          x = x0 + (x1 - x0) * (y_max_ - y0) / (y1 - y0);
          y = y_max_;
        } else if (outcodeOut & BOTTOM) {
          // point is below the clip rectangle
          x = x0 + (x1 - x0) * (y_min_ - y0) / (y1 - y0);
          y = y_min_;
        } else if (outcodeOut & RIGHT) {
          // point is to the right of clip rectangle
          y = y0 + (y1 - y0) * (x_max_- x0) / (x1 - x0);
          x = x_max_;
        } else if (outcodeOut & LEFT) {
          // point is to the left of clip rectangle
          y = y0 + (y1 - y0) * (x_min_ - x0) / (x1 - x0);
          x = x_min_;
        } else {
          MONA_ASSERT(false);
        }

        // Now we move outside point to intersection point to clip
        // and get ready for next pass.
        if (outcodeOut == outcode0) {
          x0 = x;
          y0 = y;
          outcode0 = ComputeOutCode(x0, y0);
        } else {
          x1 = x;
          y1 = y;
          outcode1 = ComputeOutCode(x1, y1);
        }
      }
    }
    if (accept) {
      //      DrawRectangle(xmin, ymin, xmax, ymax);
      //      LineSegment(x0, y0, x1, y1);
      drawLineDirect(x0, y0, x1, y1);
    }
  }

  void drawLineDirect(int x0, int y0, int x1, int y1);
  int drawStringInternal(const String& str, int x, int y, int maxWidth, bool draws);

  double x_min_;
  double x_max_;
  double y_min_;
  double y_max_;
  uint32_t rgb24_;
  int fontStyle_;
  Image* image_;
};
}

#endif // _GRAPHICS_H_INCLUDED_
