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

CommentWindow::CommentWindow(const Feed& feed)
    : facebook::Frame("Facebook"),
      body_(new TextField()),
      likes_(new TextField()),
      comment_input_(new TextField()),
      comment_button_(new ShareButton("Post")),
      icon_image_(new WebImage()),
      icon_(new ImageIcon(icon_image_)),
      comments_(feed.comments),
      feed_(feed) {
  InitIcon(feed);
  int component_y = 0;
  component_y = InitBody(feed, component_y);
  component_y = InitLikes(feed, component_y);
  component_y = InitComments(feed.comments, component_y);
  component_y = InitCommentInput(component_y);
  setBounds(80, 80, kWindowWidth, component_y);
  setBackground(monagui::Color::white);
  return;
}

CommentWindow::~CommentWindow() {
  for (Components::const_iterator i = components_to_delete_.begin();
       i != components_to_delete_.end(); ++i) {
    delete (*i);
  }
}

void CommentWindow::processEvent(Event* event) {
  if (event->getSource() == comment_button_.get() &&
      event->getType() == MouseEvent::MOUSE_RELEASED) {
    std::string message(comment_input_->getText());
    if (FacebookService::PostComment(feed_.feed_id, message)) {
      comment_input_->setText("comment published");
    } else {
      message += " : comment publis error";
      comment_input_->setText(message.c_str());
    }
  }
  if (HandleLikeButtonEvent(event)) {
    return;
  }
}

bool CommentWindow::HandleLikeButtonEvent(Event* event) {
  if (event->getType() != MouseEvent::MOUSE_RELEASED) {
    return false;
  }
  size_t index = 0;
  for (std::vector<facebook::Button*>::const_iterator i = like_buttons_.begin();
       i != like_buttons_.end(); ++i, ++index) {
    if ((*i) == event->getSource()) {
      logprintf("index=%d comments.size() %d\n", index, comments_.size());
      assert(index < comments_.size());
      if (!FacebookService::AddLike(comments_[index].comment_id)) {
        monapi_warn("add like failed");
      }
      return true;
    }
  }
  return false;
}

int CommentWindow::InitBody(const Feed& feed, int component_y) {
  body_->setBorderColor(monagui::Color::white);
  body_->setEditable(false);
  add(body_.get());
  int bodyHeight = body_->getHeightByTextAndMaxWidth(feed.text.c_str(),
                                                     kBodyMaxWidth);
  bodyHeight = std::max(bodyHeight, static_cast<int>(kBodyMinimumHeight));
  body_->setBounds(kIconMargin * 2 + kIconSize, component_y,
                   kBodyMaxWidth, bodyHeight);
  body_->setTextNoRepaint(feed.text.c_str());
  return bodyHeight + component_y;
}

void CommentWindow::InitIcon(const Feed& feed) {
  icon_->setBounds(kIconMargin, kIconMargin, kIconSize, kIconSize);
  add(icon_.get());
  icon_image_->initialize(feed.profile_image_url(),
                          feed.local_image_path());
}

int CommentWindow::InitLikes(const Feed& feed, int component_y) {
  if (feed.num_likes <= 0) {
    return component_y;
  }
  likes_->setBackground(0xffedeff4);
  likes_->setBorderColor(0xffedeff4);
  likes_->setForeground(0xff293e6a);
  likes_->setEditable(false);
  add(likes_.get());
  likes_->setBounds(kIconMargin, component_y, kWindowWidth - 25, kLikesHeight);
  char buf[64];
  snprintf(buf, sizeof(buf), "%d人", feed.num_likes);
  likes_->setTextNoRepaint(buf);
  return component_y += kLikesHeight;
}

int CommentWindow::InitComments(const Comments& comments, int component_y) {
  for (Comments::const_iterator it = comments.begin();
       it != comments.end(); ++it) {
    WebImage* comment_icon_image = new WebImage();
    ImageIcon* comment_icon = new ImageIcon(comment_icon_image);
    comment_icon->setBounds(kIconMargin, component_y, kIconSize, kIconSize);
    comment_icon_image->initialize((*it).profile_image_url(),
                                   (*it).local_image_path());

    std::string content = (*it).body;
    if ((*it).num_likes > 0) {
      char buf[16];
      snprintf(buf, sizeof(buf), "%d likes", (*it).num_likes);
      content += buf;
    }
    TextField* text_field = new TextField;
    int commentHeight =
        body_->getHeightByTextAndMaxWidth(content.c_str(),
                                          kCommentWidth);
    text_field->setTextNoRepaint(content.c_str());
    text_field->setBounds(kIconSize + kIconMargin * 2, component_y,
                          kCommentWidth, commentHeight);
    text_field->setBackground(0xffedeff4);
    text_field->setBorderColor(0xffedeff4);
    text_field->setEditable(false);

    Canvas* backGround = new Canvas();
    backGround->setBounds(0,
                          component_y,
                          kCommentWidth + kIconSize + kIconMargin * 2,
                          commentHeight + kLikesHeight);
    backGround->setBackground(0xffedeff4);

    // Keep the call order of add() to make sure backGround is drawn first.
    add(backGround);
    add(text_field);
    add(comment_icon);
    component_y += commentHeight;
    facebook::Button* like_button = new facebook::Button("like!");
    like_button->setBounds(kIconSize + kIconMargin * 2, component_y,
                           kLikeButtonWidth, kLikesHeight);
    like_button->setBackground(0xffedeff4);
    add(like_button);
    component_y += kLikesHeight;
    like_buttons_.push_back(like_button);

    components_to_delete_.push_back(text_field);
    components_to_delete_.push_back(backGround);
    components_to_delete_.push_back(like_button);
    components_to_delete_.push_back(comment_icon);
    component_y += 4;
  }
  return component_y;
}

int CommentWindow::InitCommentInput(int component_y) {
  comment_input_->setBorderColor(0xffcccccc);
  comment_input_->setBounds(0, component_y, kWindowWidth - 50, 20);
  comment_button_->setBounds(kWindowWidth - 50, component_y, 30, 20);
  add(comment_input_.get());
  add(comment_button_.get());
  return std::max(component_y + kCommentInputHeight,
                  static_cast<int>(kWindowMinimumHeight));
}
};
