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

#ifndef _TEST_TERMINAL_
#define _TEST_TERMINAL_

#include "Terminal.h"

class TestTerminal : public Terminal
{
private:
    uint32_t observer_;

    void reply(Event* event)
    {
        ASSERT(event->getType() == Event::CUSTOM_EVENT);
        int ret = MonAPI::Message::send(event->from, MSG_OK, event->header);
        ASSERT(ret == M_OK);
    }
public:
    TestTerminal(uint32_t observer, MonAPI::Stream& outStream, std::string& sharedString) : Terminal(outStream, sharedString), observer_(observer)
    {
        // // Starts test with timer
        // setTimer(50);
    }

    virtual ~TestTerminal() {}

    Button& getButton()
    {
        return *(button_.get());
    }

    TextField& getCommandField()
    {
        return *(command_.get());
    }

    std::string getCommandLine() const
    {
        return command_->getText();
    }

    void processEvent(Event* event)
    {
        if (event->getType() == Event::CUSTOM_EVENT) {
            if (event->header == MSG_TEST_CLEAR_INPUT) {
                command_->setText("");
                reply(event);
            } else if (event->header == MSG_TEST_CLEAR_OUTPUT) {
                clearOutput();
                output_->setText("");
                reply(event);
            }
        }
        Terminal::processEvent(event);
    }

    void run()
    {
        static bool first = true;
        if (first) {
            int ret = MonAPI::Message::send(observer_, MSG_STARTED);
            if (ret != M_OK) {
                monapi_fatal("message failed %d", ret);
            }
            first = false;
        }
        Terminal::run();
    }
    void buttonClick()
    {
        MouseEvent event(MouseEvent::MOUSE_RELEASED, button_.get(), 0, 0);
        processEvent(&event);
    }


    std::string getOutput() const
    {
        return output_->getText();
    }

    std::string getLastLine() const
    {
        if (lines_.empty()) {
            return "*empty*";
        } else {
            return lines_[lines_.size() - 1];
        }
    }

    void enterCommand(const std::string& command)
    {
        command_->setText(command.c_str());
        pressEnterKey();
    }

    void pressEnterKey()
    {
        KeyEvent event(KeyEvent::KEY_PRESSED, command_.get(), KeyEvent::VKEY_ENTER, 0);
        command_->processEvent(&event);
        processEvent(&event);
    }

    void pressCtrlKey(char c)
    {
        KeyEvent event(KeyEvent::KEY_PRESSED, command_.get(), c, KeyEvent::VKEY_CTRL);
        command_->processEvent(&event);
    }

    void testLSCommandReturnsLFSeperatedListOfFiles()
    {
        clearOutput();
        command_->setText("ls /APPS/");
        buttonClick();
        while (find(lines_.begin(), lines_.end(), "TEST.RAW") == lines_.end()) {
        }
        EXPECT_TRUE(true);
    }

    void testEnterKeyDownRunsLSCommand()
    {
        clearOutput();
        enterCommand("ls /APPS/");
        EXPECT_TRUE(find(lines_.begin(), lines_.end(), "TEST.RAW") != lines_.end());
    }

    void testLSCausesScrollToTheLastLine()
    {
        clearOutput();
        command_->setText("ls /APPS/");
        buttonClick();
        // On the output, we should include the last line.
        EXPECT_TRUE(strstr(output_->getText(), lines_[lines_.size() - 1].c_str()) != NULL);
    }

    void testPSShowsHeaderAndProcess()
    {
        clearOutput();
        command_->setText("ps");
        buttonClick();
        EXPECT_TRUE(strstr(output_->getText(), "tid name") != NULL);
    }

    void testCommandEnteredAppearsOnHistory()
    {
        clearOutput();
        enterCommand("ls /LIBS/");
        enterCommand("ls /USER/");

        pressCtrlKey('p');
        EXPECT_EQ(0, strcmp("ls /LIBS/", command_->getText()));

        pressCtrlKey('n');
        EXPECT_EQ(0, strcmp("ls /USER/", command_->getText()));
    }

    void test()
    {
        testLSCommandReturnsLFSeperatedListOfFiles();
        testLSCausesScrollToTheLastLine();
        testPSShowsHeaderAndProcess();
        testEnterKeyDownRunsLSCommand();
        testCommandEnteredAppearsOnHistory();
        TEST_RESULTS();
        stop();
    }
};

#endif // _TEST_TERMINAL_
