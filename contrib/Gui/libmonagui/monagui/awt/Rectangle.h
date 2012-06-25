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

#if !defined(_RECTANGLE_H_INCLUDED_)
#define _RECTANGLE_H_INCLUDED_

namespace monagui {
class Rectangle : public Object {
 private:
  int x_;
  int y_;
  int width_;
  int height_;

 public:
  Rectangle();
  Rectangle(int x, int y, int w, int h);
  virtual ~Rectangle() {}
  
  int getX() const;
  int getY() const;
  int getWidth() const;
  int getHeight() const;
  void setX(int x);
  void setY(int y);
  void setWidth(int width);
  void setHeight(int height);

  Rectangle createUnion(const Rectangle& r);
  bool isEmpty() const;
  void setBounds(int x, int y, int w, int h);
  void setLocation(int x, int y);
  void setSize(int w, int h);
};
}

#endif // _RECTANGLE_H_INCLUDED_
