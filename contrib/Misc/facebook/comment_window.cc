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

#include <stdlib.h>
#include <algorithm>

#include "./comment_window.h"
#include "./share_button.h"
#include "./feed.h"
#include "./facebook_service.h"
#include "./parser.h"

namespace facebook {


int CommentWindow::InitBody(const Feed& feed, int componentY) {
  body_->setBorderColor(monagui::Color::white);
  body_->setEditable(false);
  add(body_.get());
  int bodyHeight = body_->getHeightByTextAndMaxWidth(feed.text.c_str(),
                                                     kBodyMaxWidth);
  bodyHeight = std::max(bodyHeight, static_cast<int>(kBodyMinimumHeight));
  body_->setBounds(kIconMargin * 2 + kIconSize, componentY,
                   kBodyMaxWidth, bodyHeight);
  body_->setTextNoRepaint(feed.text.c_str());
  return bodyHeight + componentY;
}

void CommentWindow::InitIcon(const Feed& feed) {
  icon_->setBounds(kIconMargin, kIconMargin, kIconSize, kIconSize);
  add(icon_.get());
  icon_image_->initialize(feed.profile_image_url(),
                          feed.local_image_path());
}

int CommentWindow::InitLikes(const Feed& feed, int componentY) {
  if (feed.num_likes <= 0) {
    return componentY;
  }
  likes_->setBackground(0xffedeff4);
  likes_->setBorderColor(0xffedeff4);
  likes_->setForeground(0xff293e6a);
  likes_->setEditable(false);
  add(likes_.get());
  likes_->setBounds(kIconMargin, componentY, kWidth - 25, kLikesHeight);
  char buf[64];
  snprintf(buf, sizeof(buf), "%d人", feed.num_likes);
  likes_->setTextNoRepaint(buf);
  return componentY += kLikesHeight;
}

// refactor
// post_id
// delete objects
// const rename
CommentWindow::CommentWindow(const Feed& feed)
    : facebook::Frame("Facebook comment"),
      body_(new TextField()),
      likes_(new TextField()),
      comment_input_(new TextField()),
      comment_button_(new ShareButton("Post")),
      icon_image_(new WebImage()),
      icon_(new ImageIcon(icon_image_.get())),
      feed_(feed) {
  setBackground(monagui::Color::white);

  InitIcon(feed);
  int componentY = 0;
  componentY = InitBody(feed, componentY);
  componentY = InitLikes(feed, componentY);

  int i = 0;
  comments_ = feed.comments;
  for (Comments::const_iterator it = comments_.begin();
       it != comments_.end(); ++it) {
    WebImage* commentIconImage = new WebImage();
    ImageIcon* commentIcon = new ImageIcon(commentIconImage);
    std::string imageUrl("http://graph.facebook.com/");
    imageUrl += (*it).id;
    imageUrl += "/picture";
    std::string localImagePath("/USER/TEMP/");
    localImagePath += (*it).id;
    localImagePath += ".JPG";
    commentIcon->setBounds(kIconMargin, componentY, kIconSize, kIconSize);
    add(commentIcon);
    commentIconImage->initialize(imageUrl, localImagePath);
    TextField* textField = new TextField;
    const int COMMENT_WIDTH = kWidth - kIconMargin * 2 - kIconSize - 20;
    const int COMMENT_MINIMUM_HEIGHT = kIconSize + 10;
    int commentHeight =
        body_->getHeightByTextAndMaxWidth((*it).body.c_str(),
                                          COMMENT_WIDTH);
    commentHeight = commentHeight < COMMENT_MINIMUM_HEIGHT ?
        COMMENT_MINIMUM_HEIGHT :commentHeight;
    textField->setTextNoRepaint((*it).body.c_str());
    textField->setBounds(kIconSize + kIconMargin * 2, componentY,
                         COMMENT_WIDTH, commentHeight);
    componentY += commentHeight;
    textField->setBackground(0xffedeff4);
    textField->setBorderColor(0xffedeff4);
    textField->setEditable(false);
    add(textField);
    i++;
  }
  comment_input_->setBorderColor(0xffcccccc);
  comment_input_->setBounds(0, componentY, kWidth - 50, 20);
  comment_button_->setBounds(kWidth - 50, componentY, 30, 20);
  componentY += 100;
  add(comment_input_.get());
  add(comment_button_.get());
  const int MINIMUM_WINDOW_HEIGHT = 300;
  componentY = componentY < MINIMUM_WINDOW_HEIGHT ?
      MINIMUM_WINDOW_HEIGHT : componentY;
  setBounds(80, 80, kWidth, componentY);
  return;
}

void CommentWindow::processEvent(Event* event) {
  if (event->getSource() == comment_button_.get() &&
      event->getType() == MouseEvent::MOUSE_RELEASED) {
    std::string message(comment_input_->getText());
    if (FacebookService::post_comment(feed_.feed_id, message)) {
      comment_input_->setText("comment published");
    } else {
      message += " : comment publis error";
      comment_input_->setText(message.c_str());
    }
  }
}

void CommentWindow::paint(Graphics* g) {
  facebook::Frame::paint(g);
}
};
