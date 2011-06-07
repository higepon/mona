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

#ifndef _TERMINAL_
#define _TERMINAL_

#include "TextFieldWithHistory.h"
#include <monapi/StringHelper.h>
#include <algorithm>
#include <string>

class Terminal : public Frame
{
protected:
    MonAPI::scoped_ptr<TextField> command_;
    MonAPI::scoped_ptr<TextField> output_;
    MonAPI::scoped_ptr<Button> button_;
    MonAPI::scoped_ptr<Scrollbar> scrollbar_;
    MonAPI::Stream& outStream_;
    MonAPI::Strings lines_;
    std::string& sharedString_;
    uintptr_t currentLineNo_;
    uintptr_t outputNumRows_;

    enum
    {
        OUTPUT_HIGHT = 200,
        forbidden_comma
    };

public:
    Terminal(MonAPI::Stream& outStream, std::string& sharedString) :
        command_(new TextFieldWithHistory()),
        output_(new TextField()),
        button_(new Button("go")),
        scrollbar_(new Scrollbar()),
        outStream_(outStream),
        sharedString_(sharedString),
        currentLineNo_(0)
    {
        setBounds(50, 200, 300, 400);
        setTitle("Terminal");
        command_->setText("ls /APPS/");

        int height = getFontMetrics()->getHeight("H");
        outputNumRows_ = OUTPUT_HIGHT / height - 1;

        command_->setBounds(0, 0, 200, 30);
        output_->setBounds(0, 30, 250, OUTPUT_HIGHT);
        button_->setBounds(200, 0, 40, 30);
        scrollbar_->setBounds(250, 30, 16, OUTPUT_HIGHT);
        add(command_.get());
        add(output_.get());
        add(button_.get());
        add(scrollbar_.get());
        scrollbar_->setMinimum(0);
        scrollbar_->setMaximum(0);
        scrollbar_->setBlocksize(1);
    }

    virtual ~Terminal() {}


    void processEvent(Event* event)
    {
        bool runsCommand =
            (event->getType() == MouseEvent::MOUSE_RELEASED && event->getSource() == button_.get()) ||
            (event->getType() == KeyEvent::KEY_PRESSED &&
             event->getSource() == command_.get() &&
             ((KeyEvent*)event)->getKeycode() == KeyEvent::VKEY_ENTER);
        if (runsCommand) {
            if (!sendCommand(command_->getText())) {
                output_->setText("command failed");
            }
        } else if (event->getSource() == scrollbar_.get()) {
            int currentLineNo = scrollbar_->getValue();
            updateOutputView(currentLineNo);
            repaint();
        } else if (event->getType() == Event::CUSTOM_EVENT &&
                   event->header == MSG_UPDATE) {
            appendOutput(sharedString_);

            if (MonAPI::Message::send(event->from, MSG_OK, event->header) != M_OK) {
                monapi_fatal("sub thread is dead");
            }

            int currentLineNo = scrollbar_->getValue();
            updateOutputView(currentLineNo);
            repaint();
        }
    }

protected:

    void appendOutput(const std::string& content)
    {
        MonAPI::Strings lines = MonAPI::StringHelper::split("\n", content);
        for (MonAPI::Strings::const_iterator it = lines.begin(); it != lines.end(); ++it) {
            lines_.push_back(*it);
        }
        // Adjust scrollbar
        scrollbar_->setMaximum(lines_.size() - outputNumRows_);
        scrollbar_->setValue(scrollbar_->getMaximum());
        if (lines_.size() - outputNumRows_ > 0) {
            currentLineNo_ = lines_.size() - outputNumRows_;
        } else {
            currentLineNo_ = 0;
        }
    }

    void updateOutputView(int currentLineNo)
    {
        currentLineNo_ = currentLineNo;
        std::string content;
        for (uintptr_t i = 0; i + currentLineNo_ < lines_.size() && i < outputNumRows_; i++) {
            content += lines_[i + currentLineNo_];
            content += "\n";
        }
       output_->setText(content.c_str());
    }

    void clearOutput()
    {
        lines_.clear();
    }

    bool sendCommand(const std::string& command)
    {
        uint32_t tid;
        if (monapi_name_whereis("/servers/shell", tid) != M_OK) {
            return false;
        }
        MessageInfo msg;
        if (MonAPI::Message::sendReceive(&msg, tid, MSG_TEXT, outStream_.handle(), 0, 0, command.c_str()) != M_OK) {
            return false;
        }
        return true;
    }
};

#endif // _TERMINAL_
