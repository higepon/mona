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
#include <monagui.h>
#include "Terminal.h"

using namespace MonAPI;

extern void test(TerminalInfo* info);

void __fastcall stdoutStreamReader(void* arg)
{
    TerminalInfo* info = (TerminalInfo*)arg;
    scoped_array<char> buf(new char[info->outStream.capacity()]);

    // read from outStream, accumulates as string.
    // Then notifies data has come.
    for (;;) {
        uint32_t sizeRead = info->outStream.read(buf.get(), info->outStream.capacity(), true);
        info->sharedString.clear();
        info->sharedString += std::string(buf.get(), sizeRead);
        MessageInfo msg;
        if (Message::sendReceive(&msg, info->mainThread, MSG_UPDATE) != M_OK) {
            monapi_fatal("main thread is dead?");
        }
    }
}

int main(int argc, char* argv[])
{
    intptr_t ret = monapi_enable_stacktrace("/APPS/MONAGUI/TERMINAL.MAP");
    if (ret != M_OK) {
        fprintf(stderr, "terminal: stack_trace_enable failed error=%d %d.\n", ret, syscall_get_tid());
    }
    bool isTestMode = false;
    if (argc == 2 && strcmp(argv[1], "-t") == 0) {
        isTestMode = true;
    }

    TerminalInfo info;
    if (isTestMode) {
        test(&info);
    } else {
        Terminal terminal(info.outStream, info.sharedString);
        monapi_thread_create_with_arg(stdoutStreamReader, &info);
        terminal.run();
    }
    return 0;
}
