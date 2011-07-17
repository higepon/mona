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
      input_area_(new TextField()),
      share_button_(new facebook::ShareButton()),
      down_button_(new facebook::Button(">>>")),
      update_button_(new facebook::Button("Update")),
      updating_(false),
      idle_time_msec_(2000),
      offset_(0),
      is_auto_update_(true) {
  setBackground(monagui::Color::white);
  setBounds(40, 40, WIDTH, HEIGHT);
  input_area_->setBorderColor(0xffcccccc);
  int x = 5;
  int y = 5;
  input_area_->setBounds(x, y, INPUT_AREA_WIDTH, INPUT_AREA_HEIGHT);
  x += INPUT_AREA_WIDTH + BUTTON_MARGIN;
  share_button_->setBounds(x, y, BUTTON_WIDTH, BUTTON_HEIGHT);
  x += BUTTON_WIDTH + BUTTON_MARGIN;
  update_button_->setBounds(x, y, BUTTON_WIDTH, BUTTON_HEIGHT);
  x += BUTTON_WIDTH + BUTTON_MARGIN;
  down_button_->setBounds(640, 385, BUTTON_WIDTH, BUTTON_HEIGHT);
  down_button_->setFontStyle(Font::BOLD);
  add(input_area_.get());
  add(share_button_.get());
  add(down_button_.get());
  add(update_button_.get());
  setTimer(TIMER_INTERVAL_MSEC);

  y += BUTTON_HEIGHT + BUTTON_MARGIN;
  for (size_t i = 0; i < MAX_ROWS; i++) {
    FeedView* view = new FeedView(5, y + POST_HEIGHT * i, WIDTH, POST_HEIGHT);
    views_.push_back(view);
    Components c;
    view->components(&c);
    for (Components::const_iterator it = c.begin(); it != c.end(); ++it) {
      add(*it);
    }
  }
}

MainWindow::~MainWindow() {
}

void MainWindow::post_feed() {
  share_button_->setEnabled(false);
  if (FacebookService::post_feed(input_area_->getText())) {
    input_area_->setText("");
  }
  share_button_->setEnabled(true);
  update_feed_async();
}

void MainWindow::update_feed_async() {
  set_status_updating();
  int ret = MonAPI::Message::send(updater_id_, MSG_UPDATE);
  if (ret != M_OK) {
    monapi_fatal("MSG_UPDATE send failed");
  }
}

bool MainWindow::read_feed_from_file() {
  Parser parser("/USER/TEMP/fb.json");
  bool ret = parser.parse(&feeds_);
  if (!ret) {
    logprintf("err=%s\n", parser.last_error().c_str());
  }
  return ret;
}

void MainWindow::show() {
  if (!read_feed_from_file()) {
    monapi_fatal("can't read fb.data");
  }
  setup_feed_views();
}

void MainWindow::setup_feed_views(size_t offset) {
  for (size_t i = 0; i < MAX_ROWS; i++) {
    if (i + offset < feeds_.size()) {
      views_[i]->setup_from_feed(feeds_[i + offset]);
    } else {
      views_[i]->set_empty();
    }
  }
  set_status_done();
}

bool MainWindow::handle_like_button_event(Event* event) {
  if (event->getType() != MouseEvent::MOUSE_RELEASED) {
    return false;
  }
  for (FeedViews::const_iterator it = views_.begin();
       it != views_.end(); ++it) {
    if ((*it)->like_button() == event->getSource()) {
      (*it)->add_like();
      update_feed_async();
      return true;
    } else if ((*it)->comment_button() == event->getSource()) {
      (*it)->open_comment();
    }
  }
  return false;
}

void MainWindow::processEvent(Event* event) {
  if (event->getSource() == share_button_.get()) {
    if (event->getType() == MouseEvent::MOUSE_RELEASED) {
      post_feed();
    }
  } else if (handle_like_button_event(event)) {
  } else if (event->getSource() == down_button_.get()) {
    if (event->getType() == MouseEvent::MOUSE_RELEASED) {
      is_auto_update_ = false;
      setup_feed_views(++offset_);
      repaint();
    }
  } else if (event->getSource() == update_button_.get()) {
    if (event->getType() == MouseEvent::MOUSE_RELEASED) {
      is_auto_update_ = true;
      update_feed_async();
    }
  } else if (event->getType() == Event::CUSTOM_EVENT) {
    if (event->header == MSG_OK && event->from == updater_id_) {
      if (event->arg1 == M_OK) {
        show();
      } else {
        // error, just ignore and retry next.
        set_status_done();
      }
    }
  } else if (event->getType() == Event::TIMER) {
    static bool isFirstTime = true;
    if (!updating_) {
      idle_time_msec_ += TIMER_INTERVAL_MSEC;
      if (isFirstTime || (is_auto_update_ &&
                          idle_time_msec_ > UPDATE_INTERVAL_MSEC)) {
        isFirstTime = false;
        //                    logprintf("timer update feed start\n");
        update_feed_async();
      }
    }
    setTimer(TIMER_INTERVAL_MSEC);
  } else {
  }
}

void MainWindow::set_status_done() {
  update_button_->setEnabled(true);
  update_button_->setLabel("update");
  updating_ = false;
  idle_time_msec_ = 0;
  repaint();
}

void MainWindow::paint(Graphics *g) {
  facebook::Frame::paint(g);
  g->setColor(monagui::Color::gray);
  g->drawLine(5, 9 + BUTTON_HEIGHT, getWidth(),  9 + BUTTON_HEIGHT);

  for (FeedViews::const_iterator it = views_.begin();
       it != views_.end(); ++it) {
    (*it)->draw(g);
  }
}

void MainWindow::set_status_updating() {
  updating_ = true;
  update_button_->setEnabled(false);
  update_button_->setLabel("updating");
}
}
