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
#ifndef CONTRIB_MISC_FACEBOOK_MAIN_WINDOW_H_
#define CONTRIB_MISC_FACEBOOK_MAIN_WINDOW_H_

#include <vector>
#include <string>

#include "./frame.h"
#include "./feed.h"

namespace facebook {

class Button;
class FeedView;

class MainWindow : public facebook::Frame {
 public:
  explicit MainWindow(uintptr_t updater_id);
  ~MainWindow();

 private:
  uintptr_t updater_id_;
  MonAPI::scoped_ptr<TextField> input_area_;
  MonAPI::scoped_ptr<facebook::Button> share_button_;
  MonAPI::scoped_ptr<facebook::Button> down_button_;
  MonAPI::scoped_ptr<facebook::Button> updateButton_;
  typedef std::vector<std::string> strings;

  typedef std::vector<FeedView*> FeedViews;
  Feeds feeds_;
  bool updating_;
  int idle_time_msec_;
  int offset_;
  FeedViews views_;
  bool is_auto_update_;

 public:
  enum {
    BUTTON_WIDTH = 50,
    BUTTON_HEIGHT = 20,
    BUTTON_MARGIN = 5,
    INPUT_AREA_WIDTH = 300,
    INPUT_AREA_HEIGHT = BUTTON_HEIGHT,
    WIDTH = 700,
    HEIGHT = 435,
    POST_HEIGHT = 50,
    MAX_ROWS = 7,
    TIMER_INTERVAL_MSEC = 1000,
    UPDATE_INTERVAL_MSEC = 30 * 1000,
    forbidden_comma
  };

 private:
  void post_feed();
  void update_feed_async();
  bool read_feed_from_file();
  void show();
  void setup_feed_views(size_t offset = 0);
  bool handle_like_button_event(Event* event);
  void processEvent(Event* event);
  void set_status_done();
  void paint(Graphics *g);
  void set_status_updating();
};
}

#endif  // CONTRIB_MISC_FACEBOOK_MAIN_WINDOW_H_
