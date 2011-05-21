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

#ifndef _FACEBOOK_SERVICE_
#define _FACEBOOK_SERVICE_

class FacebookService
{
public:
    static bool postFeed(const std::string& text)
    {
        return executeMosh("/LIBS/MOSH/bin/fb-feed-post.sps", text);
    }

    static bool addLike(const std::string& postId)
    {
        return executeMosh("/LIBS/MOSH/bin/fb-like-post.sps", postId);
    }

private:
    static bool executeMosh(const std::string& script, const std::string& arg)
    {
        uint32_t tid;
        std::string command(MonAPI::System::getMoshPath());
        command += " ";
        command += script;
        command += " ";
        command += arg;
        int result = monapi_process_execute_file_get_tid(command.c_str(),
                                                         MONAPI_TRUE,
                                                         &tid,
                                                         MonAPI::System::getProcessStdinID(),
                                                         MonAPI::System::getProcessStdoutID());
        if (result != 0) {
            monapi_fatal("can't exec Mosh");
        }
        if (0 == monapi_process_wait_terminated(tid)) {
            return true;
        } else {
            return false;
        }
    }
};

#endif // _FACEBOOK_SERVICE_
