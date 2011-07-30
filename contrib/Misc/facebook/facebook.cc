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

#include "./updater.h"
#include "./main_window.h"
#include "./comment_window.h"
#include "./parser.h"
#include "./facebook_service.h"

static void __fastcall updaterLauncher(void* arg) {
  facebook::Updater updater;
  updater.Run();
}

static bool read_feed(const std::string& feed_id, const std::string& json_file,
                      facebook::Feed* dest, std::string* next_url) {
  facebook::Parser parser(json_file);
  facebook::Feeds feeds;
  bool ret = parser.Parse(&feeds, next_url);
  if (!ret) {
    monapi_warn("err=%s\n", parser.last_error().c_str());
    return false;
  }
  for (facebook::Feeds::const_iterator i = feeds.begin();
       i != feeds.end(); ++i) {
    if ((*i).feed_id == feed_id) {
      *dest = *i;
#ifndef OFF_LINE
      if (!facebook::FacebookService::GetComments(feed_id)) {
        fprintf(stderr, "can't read feed file");
        return -1;
      }
#endif
      facebook::Parser parser("/USER/TEMP/fb.comments.json");
      facebook::Comments comments;
      if (!parser.ParseComments(&comments)) {
        fprintf(stderr, "can't parse comments");
        return -1;
      }
      dest->comments = comments;
      return true;
    }
  }
  return false;
}

#define FACEBOOK_OAUTH_COMMAND "/APPS/W3M.APP/W3M.EX5 https://www.facebook.com/dialog/oauth?client_id=179761552055871&redirect_uri=http://lovamimi.com/login_monaos.scm&scope=publish_stream,offline_access,read_stream&display=wap" // NOLINT

int main(int argc, char* argv[]) {
  intptr_t ret = monapi_enable_stacktrace("/APPS/MONAGUI/FACEBOOK.MAP");
  if (ret != M_OK) {
    monapi_fatal("syscall_stack_trace_enable failed%d\n", ret);
  }

  if (!monapi_file_exists("/USER/FB.TOKEN")) {
    bool waits = true;
    if (!facebook::FacebookService::ExecuteCommand(
            FACEBOOK_OAUTH_COMMAND, waits)) {
      monapi_fatal("oauth2 failed");
    }
  }

  if (argc == 1) {
    uintptr_t updater_id =
        monapi_thread_create_with_arg(updaterLauncher, NULL);
    facebook::MainWindow window(updater_id);
    window.run();
  } else if (argc == 3) {
    std::string feed_id(argv[1]);
    std::string json_file(argv[2]);
    facebook::Feed feed;
    std::string next_url;
    if (read_feed(feed_id, json_file, &feed, &next_url)) {
      facebook::CommentWindow window(feed);
      window.run();
    } else {
      fprintf(stderr, "can't read feed file");
    }
  } else {
      fprintf(stderr, "facebook bad arguments");
  }
  return 0;
}
