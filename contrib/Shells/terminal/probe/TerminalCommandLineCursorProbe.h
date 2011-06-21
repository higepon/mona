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

#ifndef _TERMINAL_COMMANDLINE_CURSOR_PROBE_
#define _TERMINAL_COMMANDLINE_CURSOR_PROBE_

class TerminalCommandLineCursorProbe : public MonAPI::Probe
{
private:
    TestTerminal& terminal_;
    int lastCursor_;
    int expectedCursor_;

    void describe(std::string& d, int n)
    {
        char buf[32];
        snprintf(buf, sizeof(buf), "%d", n);
        d += "<";
        d += buf;
        d += ">";
    }

public:
    TerminalCommandLineCursorProbe(TestTerminal& terminal, int expectedCursor) :
        terminal_(terminal),
        expectedCursor_(expectedCursor)
    {
    }

    void sample()
    {
        lastCursor_ = terminal_.getCommandField().getCursor();
    }
    bool isSatisfied()
    {
        return lastCursor_ == expectedCursor_;
    }

    void describeTo(std::string& d)
    {
        describe(d, expectedCursor_);
    }

    void describeFailureTo(std::string& d)
    {
        describe(d, lastCursor_);
    }
};

#endif // _TERMINAL_COMMANDLINE_CURSOR_PROBE_
