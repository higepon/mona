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

using namespace MonAPI;

class Terminal : public Frame
{
private:
    scoped_ptr<TextField> command_;
    scoped_ptr<TextField> output_;
    scoped_ptr<Button> button_;
    Stream outStream_;

public:
    Terminal(bool isTestMode = false) :
        command_(new TextField()),
        output_(new TextField()),
        button_(new Button("go"))
    {
        setBounds(50, 200, 300, 400);
        setTitle("Terminal");
        command_->setText("ls /APPS/");
        command_->setBounds(0, 0, 200, 30);
        output_->setBounds(0, 30, 250, 200);
        button_->setBounds(200, 0, 80, 30);
        add(command_.get());
        add(output_.get());
        add(button_.get());
        if (isTestMode) {
            setTimer(50);
        }
    }

    ~Terminal() {}

    void processEvent(Event* event)
    {
        if (event->getType() == MouseEvent::MOUSE_RELEASED &&
            event->getSource() == button_.get()) {
            if (!sendCommand(command_->getText())) {
                output_->setText("command failed");
            }
        } else if (event->getType() == Event::TIMER) {
            test();
        }
    }

private:
    bool sendCommand(const std::string& command)
    {
        uint32_t tid;
        if (monapi_name_whereis("/servers/shell", tid) != M_OK) {
            return false;
        }
        MessageInfo msg;
        if (Message::sendReceive(&msg, tid, MSG_TEXT, outStream_.handle(), 0, 0, command.c_str()) != M_OK) {
            return false;
        }
        uint8_t buf[10240];
        int sizeRead = outStream_.read(buf, 10240);
        output_->setText(std::string((const char*)buf, sizeRead).c_str());
        return true;
    }

    void testLSCommandReturnsLFSeperatedListOfFiles()
    {
        output_->setText("test start");
        command_->setText("ls /APPS/");
        MouseEvent event(MouseEvent::MOUSE_RELEASED, button_.get(), 0, 0);
        processEvent(&event);
        EXPECT_TRUE(strstr(output_->getText(), "TEST.RAW\n") != NULL);
    }

    void test()
    {
        testLSCommandReturnsLFSeperatedListOfFiles();
        TEST_RESULTS();
        stop();
    }
};

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

    Terminal terminal(isTestMode);
    terminal.run();
    return 0;
}
