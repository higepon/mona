/*
 *   Copyright (c) 2012  Higepon(Taro Minowa)  <higepon@users.sourceforge.jp>
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 *   TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "monagui.h"

namespace monagui {

Rectangle::Rectangle() : x_(0),
                         y_(0),
                         width_(0),
                         height_(0) {
}

Rectangle::Rectangle(int x, int y, int w, int h) :
    x_(x),
    y_(y),
    width_(w),
    height_(h) {
}

Rectangle Rectangle::createUnion(const Rectangle& r) {
  if (r.isEmpty()) {
    return *this;
  } else if (isEmpty()) {
    return r;
  }
  int nx = std::min(x_, r.x_);
  int ny = std::min(y_, r.y_);
  int nx_max = std::max(x_ + width_, r.x_ + r.width_);
  int ny_max = std::max(y_ + height_, r.y_ + r.height_);
  return Rectangle(nx, ny, nx_max - nx, ny_max - ny);
}

bool Rectangle::isEmpty() const {
  return width_ == 0 && height_ == 0;
}

void Rectangle::setBounds(int x, int y, int w, int h) {
  setLocation(x, y);
  setSize(w, h);
}

void Rectangle::setLocation(int x, int y) {
  x_ = x;
  y_ = y;
}

void Rectangle::setSize(int w, int h) { 
  width_ = w;
  height_ = h;
}

int Rectangle::getX() const {
  return x_;
}

int Rectangle::getY() const {
  return y_;
}

int Rectangle::getWidth() const {
  return width_;
}

int Rectangle::getHeight() const {
  return height_;
}

void Rectangle::setX(int x) {
  x_ = x;
}

void Rectangle::setY(int y) {
  y_ = y;
}

void Rectangle::setWidth(int width) {
  width_ = width;
}

void Rectangle::setHeight(int height) {
  height_ = height;
}

};
