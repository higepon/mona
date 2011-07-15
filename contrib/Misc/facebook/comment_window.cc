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

#include "./comment_window.h"
#include "./share_button.h"
#include "./feed.h"
#include "./facebook_service.h"
#include "./parser.h"

namespace facebook {

CommentWindow::CommentWindow(const std::string& post_id)
    : facebook::Frame("Facebook comment"),
      body_(new TextField()),
      likes_(new TextField()),
      comment_input_(new TextField()),
      comment_button_(new ShareButton("Post")),
      icon_image_(new WebImage()),
      icon_(new ImageIcon(icon_image_.get())),
      post_id_("") {
  const int kWidth = 300;
  setBackground(monagui::Color::white);
  body_->setBorderColor(monagui::Color::white);
  body_->setEditable(false);
  add(body_.get());

  likes_->setBackground(0xffedeff4);
  likes_->setBorderColor(0xffedeff4);
  likes_->setForeground(0xff293e6a);
  likes_->setEditable(false);
  add(likes_.get());

  const int ICON_MARGIN = 5;
  const int ICON_SIZE = 30;
  icon_->setBounds(ICON_MARGIN, ICON_MARGIN, ICON_SIZE, ICON_SIZE);
  add(icon_.get());
  Feeds posts;
  if (!read_feed_from_file(&posts)) {
    body_->setTextNoRepaint("unknown post");
  }

  int componentY = 0;
  // TODO(higepon): image, field, destruct.
  for (Feeds::const_iterator it = posts.begin(); it != posts.end(); ++it) {
    if ((*it).post_id == post_id) {
      post_id_ = post_id;
      icon_image_->initialize((*it).profile_image_url(),
                              (*it).local_image_path());

      // message body
      const int BODY_MINIMUM_HEIGHT = ICON_SIZE + 10;
      const int BODY_RIGHT_MARGIN = 20;
      const int BODY_MAX_WIDTH = kWidth - ICON_MARGIN * 2
          - ICON_SIZE - BODY_RIGHT_MARGIN;
      int bodyHeight = body_->getHeightByTextAndMaxWidth((*it).text.c_str(),
                                                         BODY_MAX_WIDTH);
      bodyHeight = bodyHeight < BODY_MINIMUM_HEIGHT ?
          BODY_MINIMUM_HEIGHT : bodyHeight;
      body_->setBounds(ICON_MARGIN * 2 + ICON_SIZE,
                       componentY, BODY_MAX_WIDTH, bodyHeight);
      body_->setTextNoRepaint((*it).text.c_str());
      componentY += bodyHeight;

      // likes
      if ((*it).num_likes > 0) {
        add(likes_.get());
        componentY += ICON_MARGIN;
        const int LIKES_HEIGHT = 25;
        likes_->setBounds(ICON_MARGIN, componentY, kWidth - 25, LIKES_HEIGHT);
        componentY += LIKES_HEIGHT;
        char buf[64];
        snprintf(buf, sizeof(buf), "%d人", (*it).num_likes);
        likes_->setTextNoRepaint(buf);
      }
      int i = 0;
      comments_ = (*it).comments;
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
        commentIcon->setBounds(ICON_MARGIN, componentY, ICON_SIZE, ICON_SIZE);
        add(commentIcon);
        commentIconImage->initialize(imageUrl, localImagePath);
        TextField* textField = new TextField;
        const int COMMENT_WIDTH = kWidth - ICON_MARGIN * 2 - ICON_SIZE - 20;
        const int COMMENT_MINIMUM_HEIGHT = ICON_SIZE + 10;
        int commentHeight =
            body_->getHeightByTextAndMaxWidth((*it).body.c_str(),
                                              COMMENT_WIDTH);
        commentHeight = commentHeight < COMMENT_MINIMUM_HEIGHT ?
            COMMENT_MINIMUM_HEIGHT :commentHeight;
        textField->setTextNoRepaint((*it).body.c_str());
        textField->setBounds(ICON_SIZE + ICON_MARGIN * 2, componentY,
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
  }
  body_->setTextNoRepaint("unknown post");
}

void CommentWindow::processEvent(Event* event) {
  if (event->getSource() == comment_button_.get() &&
      event->getType() == MouseEvent::MOUSE_RELEASED) {
    std::string message(comment_input_->getText());
    if (FacebookService::post_comment(post_id_, message)) {
      comment_input_->setText("comment published");
    } else {
      message += " : comment publis error";
      comment_input_->setText(message.c_str());
    }
  }
}

bool CommentWindow::read_feed_from_file(Feeds* feeds) {
  Parser parser("/USER/TEMP/fb.json");
  bool ret = parser.parse(feeds);
  if (!ret) {
    logprintf("err=%s\n", parser.last_error().c_str());
  }
  return ret;
}

void CommentWindow::paint(Graphics* g) {
  facebook::Frame::paint(g);
}
};
