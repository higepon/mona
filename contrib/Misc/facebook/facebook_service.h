/*
 *
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

#ifndef CONTRIB_MISC_FACEBOOK_FACEBOOK_SERVICE_H_
#define CONTRIB_MISC_FACEBOOK_FACEBOOK_SERVICE_H_

#include <string>

namespace facebook {

class FacebookService {
 public:
  static std::string profile_image_url(const std::string id);
  static bool PostComment(const std::string& post_id, const std::string& text);
  static bool PostFeed(const std::string& text);
  static bool AddLike(const std::string& post_id);
  static bool GetComments(const std::string& post_id);
  static bool ExecuteCommand(const std::string& command, bool waits);
  static bool HttpGetToFile(const std::string& url,
                            const std::string& dest_file);
 private:
  static bool ExecuteMosh(const std::string& script,
                          const std::string& arg1,
                          const std::string& arg2,
                          bool waits = true);
  static bool ExecuteMosh(const std::string& script,
                          const std::string& arg,
                          bool waits = true);
};
}

#endif  // CONTRIB_MISC_FACEBOOK_FACEBOOK_SERVICE_H_
