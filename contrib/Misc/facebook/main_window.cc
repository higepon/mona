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

#include "./main_window.h"
#include "./share_button.h"
#include "./feed_view.h"
#include "./facebook_service.h"
#include "./parser.h"

namespace facebook {

MainWindow::MainWindow(uintptr_t updater_id)
    : facebook::Frame("Facebook"),
      updater_id_(updater_id),
      input_(new TextField()),
      share_button_(new facebook::ShareButton()),
      up_button_(new facebook::Button("<<<")),
      down_button_(new facebook::Button(">>>")),
      update_button_(new facebook::Button("Update")),
      updating_(false),
      idle_time_msec_(2000),
      offset_(0),
      is_auto_update_(true) {
  setBackground(monagui::Color::white);
  setBounds(40, 40, kWindowWidth, kWindowHeight);
  int component_x = kInputX;
  component_x = InitInput(component_x);
  component_x = InitShareButton(component_x);
  component_x = InitUpdateButton(component_x);

  int component_y = kInputY + kButtonHeight + kButtonMargin;
  component_y = InitFeedViews(component_y);
  component_y = InitUpDownbutton(component_y);
  setTimer(kTimerIntervalMsec);
}

MainWindow::~MainWindow() {
  for (FeedViews::const_iterator i = views_.begin(); i != views_.end(); ++i) {
    delete *i;
  }
}

void MainWindow::PostFeed() {
  share_button_->setEnabled(false);
  if (FacebookService::PostFeed(input_->getText())) {
    input_->setText("");
  } else {
    std::string input_text(input_->getText());
    input_text += " Post feed failed";
    input_->setText(input_text.c_str());
  }
  share_button_->setEnabled(true);
  UpdateFeedAsync();
}

void MainWindow::UpdateFeedAsync() {
  SetStatusUpdating();
  int ret = MonAPI::Message::send(updater_id_, MSG_UPDATE);
  if (ret != M_OK) {
    monapi_fatal("MSG_UPDATE send failed");
  }
}

bool MainWindow::ReadFeedFromFile() {
  Parser parser("/USER/TEMP/fb.json");
  bool ret = parser.Parse(&feeds_);
  if (!ret) {
    monapi_warn("err=%s\n", parser.last_error().c_str());
  }
  return ret;
}

void MainWindow::Show() {
  if (!ReadFeedFromFile()) {
    monapi_fatal("can't read fb.data");
  }
  SetupFeedViews();
}

void MainWindow::SetupFeedViews(size_t offset) {
  for (size_t i = 0; i < kMaxRows; i++) {
    if (i + offset < feeds_.size()) {
      views_[i]->SetEmpty();
      views_[i]->SetupFromFeed(feeds_[i + offset]);
    } else {
      views_[i]->SetEmpty();
    }
  }
  SetStatusDone();
}

bool MainWindow::HandleLikeButtonEvent(Event* event) {
  if (event->getType() != MouseEvent::MOUSE_RELEASED) {
    return false;
  }
  for (FeedViews::const_iterator it = views_.begin();
       it != views_.end(); ++it) {
    if ((*it)->like_button() == event->getSource()) {
      (*it)->AddLike();
      UpdateFeedAsync();
      return true;
    } else if ((*it)->comment_button() == event->getSource()) {
      (*it)->OpenComment();
    }
  }
  return false;
}

void MainWindow::processEvent(Event* event) {
  if (event->getSource() == share_button_.get()) {
    if (event->getType() == MouseEvent::MOUSE_RELEASED) {
      PostFeed();
    }
  } else if (HandleLikeButtonEvent(event)) {
  } else if (event->getSource() == down_button_.get()) {
    if (event->getType() == MouseEvent::MOUSE_RELEASED) {
      is_auto_update_ = false;
      SetupFeedViews(++offset_);
      repaint();
    }
  } else if (event->getSource() == up_button_.get()) {
    if (event->getType() == MouseEvent::MOUSE_RELEASED) {
      is_auto_update_ = false;
      if (--offset_ < 0) {
        offset_ = 0;
      }
      SetupFeedViews(offset_);
      repaint();
    }
  } else if (event->getSource() == update_button_.get()) {
    if (event->getType() == MouseEvent::MOUSE_RELEASED) {
      is_auto_update_ = true;
      UpdateFeedAsync();
    }
  } else if (event->getType() == Event::CUSTOM_EVENT) {
    if (event->header == MSG_OK && event->from == updater_id_) {
      if (event->arg1 == M_OK) {
        Show();
      } else {
        // error, just ignore and retry next.
        SetStatusDone();
      }
    }
  } else if (event->getType() == Event::TIMER) {
    static bool is_first_time = true;
    if (!updating_) {
      idle_time_msec_ += kTimerIntervalMsec;
      if (is_first_time || (is_auto_update_ &&
                          idle_time_msec_ > kUpdateInterbalMsec)) {
        is_first_time = false;
        UpdateFeedAsync();
      }
    }
    setTimer(kTimerIntervalMsec);
  } else {
  }
}

void MainWindow::SetStatusDone() {
  update_button_->setEnabled(true);
  update_button_->setLabel("update");
  updating_ = false;
  idle_time_msec_ = 0;
  repaint();
}

void MainWindow::paint(Graphics* g) {
  facebook::Frame::paint(g);
  g->setColor(monagui::Color::gray);
  g->drawLine(5, 9 + kButtonHeight, getWidth(),  9 + kButtonHeight);

  for (FeedViews::const_iterator it = views_.begin();
       it != views_.end(); ++it) {
    (*it)->Draw(g);
  }
}

void MainWindow::SetStatusUpdating() {
  updating_ = true;
  update_button_->setEnabled(false);
  update_button_->setLabel("updating");
}

int MainWindow::InitInput(int component_x) {
  input_->setBorderColor(0xffcccccc);
  input_->setBounds(component_x, kInputY, kInputWidth, kInputHeight);
  add(input_.get());
  return component_x + kInputWidth;
}

int MainWindow::InitShareButton(int component_x) {
  share_button_->setBounds(component_x + kButtonMargin, kInputY,
                           kButtonWidth, kButtonHeight);
  add(share_button_.get());
  return component_x + kButtonWidth;
}

int MainWindow::InitUpdateButton(int component_x) {
  update_button_->setBounds(component_x + kButtonHeight, kInputY,
                            kButtonWidth, kButtonHeight);
  add(update_button_.get());
  return component_x + kButtonWidth;
}

int MainWindow::InitFeedViews(int component_y) {
  int this_y = component_y;
  for (size_t i = 0; i < kMaxRows; i++) {
    FeedView* view =
        new FeedView(kInputX, this_y, kWindowWidth, kPostHeight);
    views_.push_back(view);
    Components c;
    view->SetComponents(&c);
    for (Components::const_iterator it = c.begin(); it != c.end(); ++it) {
      add(*it);
    }
    this_y += kPostHeight;
  }
  return this_y;
}

int MainWindow::InitUpDownbutton(int component_y) {
  down_button_->setBounds(kDownButtonX, component_y + kButtonMargin,
                          kButtonWidth, kButtonHeight);
  down_button_->setFontStyle(Font::BOLD);
  add(down_button_.get());

  up_button_->setBounds(kUpButtonX, component_y + kButtonMargin,
                          kButtonWidth, kButtonHeight);
  up_button_->setFontStyle(Font::BOLD);
  add(up_button_.get());
  return component_y + kButtonHeight;
}
}
