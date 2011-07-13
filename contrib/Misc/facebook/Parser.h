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

#ifndef _PARSER_
#define _PARSER_

#include <string>
#include "ext/picojson.h"

namespace facebook {

class Parser
{
private:
    const char* file_;
    std::string lastError_;
public:
    Parser(const char* file) : file_(file)
    {
    }

    const std::string& lastError() const
    {
        return lastError_;
    }

    bool parse(Feeds& destFeeds)
    {
        MonAPI::scoped_ptr<MonAPI::SharedMemory> shm(monapi_file_read_all(file_));
        if (shm.get() == NULL) {
            lastError_ = "read file error";
            return false;
        }

        std::string errors;
        picojson::value v;
        picojson::parse(v, shm->data(), shm->data() + shm->size(), &errors);
        if (errors.empty() && v.is<picojson::object>()) {
            picojson::object obj = v.get<picojson::object>();
            picojson::value expectedComments = obj["data"];
            if (obj["data"].is<picojson::array>()) {
                picojson::array feeds = obj["data"].get<picojson::array>();
                for (picojson::array::const_iterator it = feeds.begin(); it != feeds.end(); ++it) {
                    if ((*it).is<picojson::object>()) {
                        picojson::object feed = (*it).get<picojson::object>();
                        std::string message = feed["message"].to_str();
                        std::string postId = feed["id"].to_str();
                        if (feed["from"].is<picojson::object>()) {
                            picojson::object from = feed["from"].get<picojson::object>();
                            std::string name = from["name"].to_str();
                            std::string id = from["id"].to_str();
                            int numLikes = 0;
                            if (feed["likes"].is<picojson::object>()) {
                                picojson::object likes = feed["likes"].get<picojson::object>();
                                if (likes["count"].is<double>()) {
                                    numLikes = likes["count"].get<double>();
                                }
                            }
                            Comments comments;
                            if (feed["comments"].is<picojson::object>()) {
                                picojson::object cs = feed["comments"].get<picojson::object>();
                                if (cs["data"].is<picojson::array>()) {
                                    picojson::array commentPosts = cs["data"].get<picojson::array>();
                                    for (picojson::array::const_iterator i = commentPosts.begin(); i != commentPosts.end(); ++i) {
                                        if ((*i).is<picojson::object>()) {
                                            picojson::object comment = (*i).get<picojson::object>();
                                            std::string message = comment["message"].to_str();
                                            if (comment["from"].is<picojson::object>()) {
                                                picojson::object from = comment["from"].get<picojson::object>();
                                                std::string fromId = from["id"].to_str();
                                                std::string fromName = from["name"].to_str();
                                                comments.push_back(Comment(fromId, message));
                                            } else {
                                                lastError_ = "comment[from] is not hash";
                                                return false;
                                            }
                                        } else {
                                            lastError_ = "comment is not hash";
                                            return false;
                                        }
                                    }
                                }
                            }
                            destFeeds.push_back(Feed(id, name, message, numLikes, postId, comments.size() /* todo */, comments));
                        } else {
                            lastError_ = "post[from] is not hash";
                            return false;
                        }

                    } else {
                        lastError_ = "one of elements of posts is not hash";
                    }

                }
            } else {
                lastError_ = "picojson::value of key \"data\" is not array";
            }
        }
        return true;
    }
};

};

#endif // _PARSER_
