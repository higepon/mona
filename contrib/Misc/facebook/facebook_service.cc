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

#include "./facebook_service.h"

#include <monapi.h>

namespace facebook {

std::string FacebookService::profile_image_url(const std::string id) {
  std::string ret = "http://graph.facebook.com/";
  ret += id;
  ret += "/picture";
  return ret;
}

bool FacebookService::PostComment(const std::string& post_id,
                                  const std::string& text) {
  return ExecuteMosh("/LIBS/MOSH/bin/fb-comment-post.sps", post_id, text);
}

bool FacebookService::PostFeed(const std::string& text) {
  return ExecuteMosh("/LIBS/MOSH/bin/fb-feed-post.sps", text);
}

bool FacebookService::AddLike(const std::string& post_id) {
  return ExecuteMosh("/LIBS/MOSH/bin/fb-like-post.sps", post_id, false);
}

bool FacebookService::ExecuteCommand(const std::string& command, bool waits) {
  uint32_t tid;
  int result = monapi_process_execute_file_get_tid(
      command.c_str(),
      MONAPI_TRUE,
      &tid,
      MonAPI::System::getProcessStdinID(),
      MonAPI::System::getProcessStdoutID());
  if (result != 0) {
    monapi_fatal("can't exec Mosh");
  }
  if (waits) {
    if (0 == monapi_process_wait_terminated(tid)) {
      return true;
    } else {
      return false;
    }
  } else {
    return true;
  }
}

bool FacebookService::ExecuteMosh(const std::string& script,
                                  const std::string& arg1,
                                  const std::string& arg2,
                                  bool waits /* = true */) {
  std::string command(MonAPI::System::getMoshPath());
  command += " ";
  command += script;
  command += " \"";
  command += arg1;
  command += "\" ";
  command += "\"";
  command += arg2;
  command += "\"";
  return ExecuteCommand(command, waits);
}

bool FacebookService::ExecuteMosh(const std::string& script,
                                  const std::string& arg,
                                  bool waits /*  = true */) {
  std::string command(MonAPI::System::getMoshPath());
  command += " ";
  command += script;
  command += " \"";
  command += arg;
  command += "\"";
  return ExecuteCommand(command, waits);
}
}
