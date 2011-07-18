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
  virtual ~MainWindow();

 private:
  uintptr_t updater_id_;
  MonAPI::scoped_ptr<TextField> input_;
  MonAPI::scoped_ptr<facebook::Button> share_button_;
  MonAPI::scoped_ptr<facebook::Button> down_button_;
  MonAPI::scoped_ptr<facebook::Button> update_button_;
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
    kButtonWidth = 50,
    kButtonHeight = 20,
    kButtonMargin = 5,
    kInputX = 5,
    kInputY = 5,
    kInputWidth = 300,
    kInputHeight = kButtonHeight,
    kWindowWidth = 700,
    kWindowHeight = 435,
    kPostHeight = 50,
    kDownButtonX = 640,
    kMaxRows = 7,
    kTimerIntervalMsec = 1000,
    kUpdateInterbalMsec = 30 * 1000,
    forbidden_comma
  };

 private:
  void paint(Graphics *g);
  void processEvent(Event* event);

  int InitInput(int component_x);
  int InitShareButton(int component_x);
  int InitUpdateButton(int component_x);
  int InitFeedViews(int component_y);
  int InitDownbutton(int component_y);

  void PostFeed();
  void UpdateFeedAsync();
  bool ReadFeedFromFile();
  void Show();
  void SetupFeedViews(size_t offset = 0);
  bool HandleLikeButtonEvent(Event* event);
  void SetStatusDone();
  void SetStatusUpdating();
};
}

#endif  // CONTRIB_MISC_FACEBOOK_MAIN_WINDOW_H_
