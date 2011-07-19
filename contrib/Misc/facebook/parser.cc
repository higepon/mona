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

#include "./parser.h"
#include "./ext/picojson.h"

namespace facebook {

Parser::Parser(const std::string& file) : file_(file) {
}

const std::string& Parser::last_error() const {
  return last_error_;
}

bool Parser::ParseComments(Comments* dest_comments) {
  MonAPI::scoped_ptr<MonAPI::SharedMemory>
      shm(monapi_file_read_all(file_.c_str()));
  if (shm.get() == NULL) {
    last_error_ = "read file error";
    return false;
  }
  std::string errors;
  picojson::value v;
  picojson::parse(v, shm->data(), shm->data() + shm->size(), &errors);
  if (!errors.empty() || !v.is<picojson::object>()) {
    return false;
  }
  picojson::object obj = v.get<picojson::object>();
  picojson::value expectedComments = obj["data"];
  if (!obj["data"].is<picojson::array>()) {
    last_error_ = "picojson::value of key \"data\" is not array";
    return false;
  }
  picojson::array comments = obj["data"].get<picojson::array>();
  for (picojson::array::const_iterator i = comments.begin();
       i != comments.end(); ++i) {
    if (!(*i).is<picojson::object>()) {
      last_error_ = "one of elements of comments is not hash";
      return false;
    }
    picojson::object comment = (*i).get<picojson::object>();
    std::string comment_id = comment["id"].to_str();
    std::string message = comment["message"].to_str();
    if (comment["from"].is<picojson::object>()) {
      picojson::object from = comment["from"].get<picojson::object>();
      std::string fromId = from["id"].to_str();
      std::string fromName = from["name"].to_str();
      int num_likes = 0;
      if (comment["likes"].is<double>()) {
        num_likes = comment["likes"].get<double>();
      }
      dest_comments->push_back(Comment(fromId, message,
                                       comment_id, num_likes));
    } else {
      last_error_ = "comment[from] is not hash";
      return false;
    }
  }
  return true;
}

bool Parser::Parse(Feeds* dest_feeds) {
  MonAPI::scoped_ptr<MonAPI::SharedMemory>
      shm(monapi_file_read_all(file_.c_str()));
  if (shm.get() == NULL) {
    last_error_ = "read file error";
    return false;
  }

  std::string errors;
  picojson::value v;
  picojson::parse(v, shm->data(), shm->data() + shm->size(), &errors);
  if (!errors.empty() || !v.is<picojson::object>()) {
    return false;
  }
  picojson::object obj = v.get<picojson::object>();
  picojson::value expectedComments = obj["data"];
  if (!obj["data"].is<picojson::array>()) {
    last_error_ = "picojson::value of key \"data\" is not array";
    return false;
  }
  picojson::array feeds = obj["data"].get<picojson::array>();
  for (picojson::array::const_iterator it = feeds.begin();
       it != feeds.end(); ++it) {
    if (!(*it).is<picojson::object>()) {
      last_error_ = "one of elements of posts is not hash";
      return false;
    }
    picojson::object feed = (*it).get<picojson::object>();
    std::string message = feed["message"].to_str();
    std::string post_id = feed["id"].to_str();
    if (!feed["from"].is<picojson::object>()) {
      last_error_ = "post[from] is not hash";
      return false;
    }
    picojson::object from = feed["from"].get<picojson::object>();
    std::string name = from["name"].to_str();
    std::string id = from["id"].to_str();
    int num_likes = 0;
    if (feed["likes"].is<picojson::object>()) {
      picojson::object likes = feed["likes"].get<picojson::object>();
      if (likes["count"].is<double>()) {
        num_likes = likes["count"].get<double>();
      }
    }
    Comments comments;
    if (feed["comments"].is<picojson::object>()) {
      picojson::object cs = feed["comments"].get<picojson::object>();
      if (cs["data"].is<picojson::array>()) {
        picojson::array commentPosts = cs["data"].get<picojson::array>();
        for (picojson::array::const_iterator i = commentPosts.begin();
             i != commentPosts.end(); ++i) {
          if ((*i).is<picojson::object>()) {
            picojson::object comment = (*i).get<picojson::object>();
            std::string comment_id = comment["id"].to_str();
            std::string message = comment["message"].to_str();
            if (comment["from"].is<picojson::object>()) {
              picojson::object from = comment["from"].get<picojson::object>();
              std::string fromId = from["id"].to_str();
              std::string fromName = from["name"].to_str();
              int num_likes = 0;
              if (comment["likes"].is<double>()) {
                num_likes = comment["likes"].get<double>();
              }
              comments.push_back(Comment(fromId, message,
                                         comment_id, num_likes));
            } else {
              last_error_ = "comment[from] is not hash";
              return false;
            }
          }
        }
      }
    }

    dest_feeds->
        push_back(Feed(id, name, message,
                       num_likes, post_id, comments.size() /* todo */,
                       comments));
  }
  return true;
}
}
