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
#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>
#include "Terminal.h"
#include "TestTerminal.h"

using namespace MonAPI;

static Stream* outStream;
static Terminal* terminal;
static std::string sharedString;

static void __fastcall stdoutStreamReader(void* mainThread)
{
    scoped_array<char> buf(new char[outStream->capacity()]);

    // read from outStream, accumulates as string.
    // Then notifies data has come.
    for (;;) {
        uint32_t sizeRead = outStream->read(buf.get(), outStream->capacity(), true);
        sharedString.clear();
        sharedString += std::string(buf.get(), sizeRead);
        MessageInfo info;
        if (Message::sendReceive(&info, (uint32_t)mainThread, MSG_UPDATE) != M_OK) {
            monapi_fatal("main thread is dead?");
        }
    }
}

class Robot
{
public:
    void mouseMove(int x, int y)
    {
        uint32_t tid;
        if (monapi_name_whereis("/servers/mouse", tid) != M_OK) {
            monapi_warn("mouse server not found");
            return;
        }

        MessageInfo msg;
        if (Message::sendReceive(&msg, tid, MSG_MOUSE_SET_CURSOR_POSITION, x, y) != M_OK) {
            monapi_warn("mouse server is dead?");
            return;
        }
    }

    void keyPress(int keycode)
    {
        uint32_t tid;
        if (monapi_name_whereis("/servers/keyboard", tid) != M_OK) {
            monapi_warn("keyboard server not found");
            return;
        }

        MessageInfo msg;
        if (Message::sendReceive(&msg, tid, MSG_KEY_PRESS, keycode) != M_OK) {
            monapi_warn("keyboard server is dead?");
            return;
        }
    }

    void keyRelease(int keycode)
    {
        uint32_t tid;
        if (monapi_name_whereis("/servers/keyboard", tid) != M_OK) {
            monapi_warn("keyboard server not found");
            return;
        }

        MessageInfo msg;
        if (Message::sendReceive(&msg, tid, MSG_KEY_RELEASE, keycode) != M_OK) {
            monapi_warn("keyboard server is dead?");
            return;
        }
    }

    // todo refacor
    void mousePress()
    {
        uint32_t tid;
        if (monapi_name_whereis("/servers/mouse", tid) != M_OK) {
            monapi_warn("mouse server not found");
            return;
        }

        MessageInfo msg;
        if (Message::sendReceive(&msg, tid, MSG_MOUSE_PRESS) != M_OK) {
            monapi_warn("mouse server is dead?");
            return;
        }
    }

    void mouseRelease()
    {
        uint32_t tid;
        if (monapi_name_whereis("/servers/mouse", tid) != M_OK) {
            monapi_warn("mouse server not found");
            return;
        }

        MessageInfo msg;
        if (Message::sendReceive(&msg, tid, MSG_MOUSE_RELEASE) != M_OK) {
            monapi_warn("mouse server is dead?");
            return;
        }
    }
};

static void waitMainThreadStartup()
{

}

static void __fastcall robotThread(void* arg)
{
    waitMainThreadStartup();
    Robot robot;
    robot.mouseMove(200, 240);
    for (;;) {
        robot.mousePress();
        sleep(300);
        robot.mouseRelease();
        robot.keyPress('a');
    }
}

int main(int argc, char* argv[])
{
    const char* MAP_FILE_PATH  = "/APPS/MONAGUI/TERMINAL.MAP";
    uint32_t pid = syscall_get_pid();
    intptr_t ret = syscall_stack_trace_enable(pid, MAP_FILE_PATH);
    if (ret != M_OK) {
        fprintf(stderr, "terminal: stack_trace_enable failed error=%d %d.\n", ret, syscall_get_tid());
    }
    bool isTestMode = false;
    if (argc == 2 && strcmp(argv[1], "-t") == 0) {
        isTestMode = true;
    }

    outStream = new Stream;
    terminal = isTestMode ? new TestTerminal(*outStream, sharedString) : new Terminal(*outStream, sharedString);
    uintptr_t mainThread = System::getThreadID();
    monapi_thread_create_with_arg(stdoutStreamReader, (void*)mainThread);
    monapi_thread_create_with_arg(robotThread, (void*)mainThread);
    Rectangle r = terminal->getButtonAbsoluteBounds();
    logprintf("r.x=%d r.y=%d", r.x, r.y);
    terminal->run();
    delete outStream;
    delete terminal;
    return 0;
}
