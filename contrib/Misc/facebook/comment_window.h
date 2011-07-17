/*
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
#ifndef CONTRIB_MISC_FACEBOOK_COMMENT_WINDOW_H_
#define CONTRIB_MISC_FACEBOOK_COMMENT_WINDOW_H_

#include <vector>
#include <string>

#include "./frame.h"
#include "./comment.h"
#include "./feed.h"
#include "./share_button.h"

namespace facebook {

class CommentWindow : public facebook::Frame {
 public:
  explicit CommentWindow(const Feed& feed);
  virtual ~CommentWindow();
  void processEvent(Event* event);

 private:
  enum ComponentLayout {
    kWindowWidth = 300,
    kWindowMinimumHeight = 300,
    kIconMargin = 5,
    kIconSize = 30,
    kBodyMinimumHeight = kIconSize + 10,
    kBodyRightMargin = 20,
    kBodyMaxWidth =
      kWindowWidth - kIconMargin * 2 - kIconSize - kBodyRightMargin,
    kLikesHeight = 25,
    kCommentWidth = kWindowWidth - kIconMargin * 2 - kIconSize - 20,
    kCommentInputHeight = 100,
    kLikeButtonHeight = 30,
    kLikeButtonWidth = 30
  };

  int InitBody(const Feed& feed, int component_y);
  void InitIcon(const Feed& feed);
  int InitLikes(const Feed& feed, int component_y);
  int InitComments(const Comments& comments, int component_y);
  int InitCommentInput(int component_y);
  bool HandleLikeButtonEvent(Event* e);

  MonAPI::scoped_ptr<TextField> body_;
  MonAPI::scoped_ptr<TextField> likes_;
  MonAPI::scoped_ptr<TextField> comment_input_;
  MonAPI::scoped_ptr<ShareButton> comment_button_;
  MonAPI::scoped_ptr<WebImage> icon_image_;
  MonAPI::scoped_ptr<ImageIcon> icon_;
  std::vector<TextField*> comment_fields_;
  std::vector<ImageIcon*> comment_icons_;
  std::vector<facebook::Button*> like_buttons_;
  Comments comments_;
  const Feed& feed_;
};
}
#endif  // CONTRIB_MISC_FACEBOOK_COMMENT_WINDOW_H_
