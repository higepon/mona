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

#ifndef _MONAPI_POLLER_
#define _MONAPI_POLLER_

namespace MonAPI {

class Poller {
private:
    int timeoutMillis_;
    int pollDellayMillis_;

public:
    Poller(int timeoutMillis, int pollDellayMillis) :
        timeoutMillis_(timeoutMillis),
        pollDellayMillis_(pollDellayMillis)
    {
    }

    bool check(Probe& probe, std::string& description)
    {
        uint64_t now = MonAPI::Date::nowInMsec();
        uint64_t end = now + timeoutMillis_;
        while (!probe.isSatisfied()) {
            if (now > end) {
                description += describeFailureOf(probe);
                return false;
            }
            sleep(pollDellayMillis_);
            now = MonAPI::Date::nowInMsec();
            probe.sample();
        }
        return true;
    }

private:
    virtual std::string describeFailureOf(Probe& probe)
    {
        std::string description;
        description += "\n    Tried to find:\n        ";
        probe.describeTo(description);
        description += "\n    but:\n    ";
        probe.describeFailureTo(description);
        return description;
    }
};

};

#endif // _MONAPI_POLLER_
