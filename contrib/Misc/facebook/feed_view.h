/*
 *
 *   Copyright (c) 2011  Higepon(Taro Minowa)  <higepon@users.sourceforge.jp>
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

#ifndef CONTRIB_MISC_FACEBOOK_FEED_VIEW_H_
#define CONTRIB_MISC_FACEBOOK_FEED_VIEW_H_

#include <monagui.h>
#include <vector>
#include <string>
#include "./feed.h"

namespace facebook {

typedef std::vector<Component*> Components;

class FeedView {
 public:
  FeedView(int x, int y, int w, int h);
  virtual ~FeedView();

  void SetComponents(Components* ret);
  void SetImagePath(const std::string& uri, const std::string& path);
  void SetText(const std::string& text);
  void SetupFromFeed(const Feed& feed);
  void SetEmpty();
  void Draw(Graphics* g);

  Button* like_button() {
    return like_button_.get();
  }

  Button* comment_button() {
    return comment_button_.get();
  }

  void OpenComment();
  void AddLike();
  std::string FoldLine(const std::string& line, size_t max_line_length);

 private:
  int InitIcon(int component_x, int component_y);
  int InitBodyText(int component_x, int component_y, int w, int h);
  int InitLike(int component_x, int component_y);
  int InitCommentButton(int component_x, int component_y);

  enum {
    kMargin = 5,
    kIconMarginLeft = 5,
    kIconMarginTop = 5,
    kIconHeight = 30,
    kIconWidth = 30,
    kLikeButtonWidth = 35,
    kLikeButtonHeight = 20,
    kCommentButtonWidth = 60,
    kCommentButtonHeight = kLikeButtonHeight,
    kLikeButtonMarginTop = 2
  };
  const int x_;
  const int y_;
  const int w_;
  const int h_;
  MonAPI::scoped_ptr<Button> like_button_;
  MonAPI::scoped_ptr<Button> comment_button_;
  MonAPI::scoped_ptr<TextField> text_;
  MonAPI::scoped_ptr<ImageIcon> icon_;
  std::string feed_id_;

  DISALLOW_COPY_AND_ASSIGN(FeedView);
};
}

#endif  // CONTRIB_MISC_FACEBOOK_FEED_VIEW_H_
