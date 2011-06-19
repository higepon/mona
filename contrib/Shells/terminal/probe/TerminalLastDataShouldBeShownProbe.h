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

#ifndef _TERMINAL_LAST_DATA_SHOULD_BE_SHOWN_PROBE_
#define _TERMINAL_LAST_DATA_SHOULD_BE_SHOWN_PROBE_

class TerminalLastDataShouldBeShownProbe : public MonAPI::Probe
{
private:
    TestTerminal& terminal_;
    std::string lastContent_;
    std::string lastLine_;
public:
    TerminalLastDataShouldBeShownProbe(TestTerminal& terminal) :
        terminal_(terminal)
    {
    }

    void sample()
    {
        lastContent_ = terminal_.getOutput();
        lastLine_ = terminal_.getLastLine();
    }
    bool isSatisfied()
    {
        bool ret = lastContent_.find(lastLine_) != std::string::npos;
        return ret;
    }

    void describeTo(std::string& d)
    {
        d += "    last line<";
        d += lastLine_;
        d += ">";
    }

    void describeFailureTo(std::string& d)
    {
        d += "<";
        d += lastContent_;
        d += "> ";
    }
};

#endif // _TERMINAL_LAST_DATA_SHOULD_BE_SHOWN_PROBE_
