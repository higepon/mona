/*
 * test.cpp -
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

#include <monagui.h>
#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>
#include "TestTerminal.h"
#include "probe/TerminalCommandLineProbe.h"
#include "probe/TerminalOutputProbe.h"
#include "probe/TerminalLastDataShouldBeShownProbe.h"
using namespace MonAPI;

class Helper
{
public:
    static uintptr_t waitSubThread(uintptr_t id)
    {
        MessageInfo dst, src;
        src.header = MSG_STARTED;
        src.from = id;
        int ret = Message::receive(&dst, &src, Message::equalsFromHeader);
        ASSERT_EQ(M_OK, ret);
        uintptr_t tid = dst.from;
        return tid;
    }

    static void stopSubThread(uintptr_t id)
    {
        ASSERT_EQ(M_OK, Message::send(id, MSG_STOP));
    }
};


TestTerminal* testTerminal;
uintptr_t terminalThread;
extern void __fastcall stdoutStreamReader(void* arg);
static void __fastcall testTerminalThread(void* arg)
{
    TerminalInfo* info = (TerminalInfo*)arg;
    terminalThread = System::getThreadID();
    uint32_t mainThread = info->mainThread;
    info->mainThread = terminalThread;
    monapi_thread_create_with_arg(stdoutStreamReader, info);
    testTerminal = new TestTerminal(mainThread, *info);
    testTerminal->run();
    delete testTerminal;
}

static void testInputSpace()
{
    MonaGUIRobot r;
    ASSERT_EQ(M_OK, MUnitService::clearInput(terminalThread));
    r.input(testTerminal->getCommandField(), " ");
    TerminalCommandLineProbe probe(*testTerminal, " ");
    ASSERT_EVENTUALLY(probe);
}

static void enterCommandAndClickButton(const std::string& command)
{
    MonaGUIRobot r;
    ASSERT_EQ(M_OK, MUnitService::clearInput(terminalThread));
    r.input(testTerminal->getCommandField(), command.c_str());
    r.click(testTerminal->getButton());
}

static void testLSCommandReturnsLFSeperatedListOfFiles()
{
    enterCommandAndClickButton("ls /APPS/");
    TerminalOutputProbe probe(*testTerminal, "HELLO.EX5");
    ASSERT_EVENTUALLY(probe);
}


static void testPSShowsHeaderAndProcess()
{
    enterCommandAndClickButton("ps");
    TerminalOutputProbe probe(*testTerminal, "tid name");
    ASSERT_EVENTUALLY(probe);
}

static void testLSCausesScrollToTheLastLine()
{
    enterCommandAndClickButton("ls /APPS/");
    TerminalLastDataShouldBeShownProbe probe(*testTerminal);
    ASSERT_EVENTUALLY(probe);
}

static void enterCommand(const std::string& command)
{
    ASSERT_EQ(M_OK, MUnitService::clearInput(terminalThread));
    ASSERT_EQ(M_OK, MUnitService::clearOutput(terminalThread));

    MonaGUIRobot r;
    r.input(testTerminal->getCommandField(), command.c_str());
    r.keyPress(Keys::Enter);
    r.keyRelease(Keys::Enter);
}

static void testEnterKeyDownRunsLSCommand()
{
    enterCommand("ls /APPS/");
    TerminalLastDataShouldBeShownProbe probe(*testTerminal);
    ASSERT_EVENTUALLY(probe);
}

static void testCommandEnteredAppearsOnHistory()
{
    enterCommand("ls /LIBS/");
    TerminalOutputProbe probe(*testTerminal, "GUI.DL5");
    ASSERT_EVENTUALLY(probe);

    enterCommand("ls /USER/");
    TerminalOutputProbe probe2(*testTerminal, "TEMP");
    ASSERT_EVENTUALLY(probe2);

    MonaGUIRobot r;
    r.keyPress('p', KEY_MODIFIER_CTRL);
    TerminalCommandLineProbe probe3(*testTerminal, "ls /LIBS/");
    ASSERT_EVENTUALLY(probe3);

    r.keyPress('n', KEY_MODIFIER_CTRL);
    TerminalCommandLineProbe probe4(*testTerminal, "ls /USER/");
    ASSERT_EVENTUALLY(probe4);
}

static void testOutputOfCalledProcessIsShown()
{
    enterCommand("/APPS/HELLO.EX5");
    TerminalOutputProbe probe(*testTerminal, "Hello, World");
    ASSERT_EVENTUALLY(probe);
}

static void testCanCallProcessWithoutPath()
{
    enterCommand("HELLO.EX5");
    TerminalOutputProbe probe(*testTerminal, "Hello, World");
    ASSERT_EVENTUALLY(probe);
}

static void testCallProcessErrorIsShown()
{
    enterCommand("HOGE");
    TerminalOutputProbe probe(*testTerminal, "call-process failed : (HOGE)");
    ASSERT_EVENTUALLY(probe);
}

static void testTextFieldEmacsKeybindCtrlAHeadOfLine()
{
    MonaGUIRobot r;
    ASSERT_EQ(M_OK, MUnitService::clearInput(terminalThread));
    r.input(testTerminal->getCommandField(), "abc");
    TerminalCommandLineProbe probe1(*testTerminal, "abc");
    ASSERT_EVENTUALLY(probe1);

    r.keyPress('a', KEY_MODIFIER_CTRL);
    r.input(testTerminal->getCommandField(), "def");
    TerminalCommandLineProbe probe2(*testTerminal, "defabc");
    ASSERT_EVENTUALLY(probe2);
}

static void testAll()
{
    testInputSpace();
    testLSCommandReturnsLFSeperatedListOfFiles();
    testPSShowsHeaderAndProcess();
    testLSCausesScrollToTheLastLine();
    testEnterKeyDownRunsLSCommand();
    testCommandEnteredAppearsOnHistory();
    testOutputOfCalledProcessIsShown();
    testCanCallProcessWithoutPath();
    testCallProcessErrorIsShown();
    testTextFieldEmacsKeybindCtrlAHeadOfLine();
    TEST_RESULTS();
}

void test(TerminalInfo* info)
{
    uint32_t id = monapi_thread_create_with_arg(testTerminalThread, info);
    Helper::waitSubThread(id);
    testAll();
    Helper::stopSubThread(id);
}
