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

#ifndef _TEXTFIELD_WITH_HISTORY_
#define _TEXTFIELD_WITH_HISTORY_

#include <monapi/StringHelper.h>
#include <algorithm>

class TextFieldWithHistory : public TextField
{
private:
    MonAPI::Strings histories_;
    int historyIndex_;
    std::string historyPath_;

    void addToHistories(const std::string& text)
    {
        if (text.empty()) {
            return;
        }
        histories_.push_back(text);
        historyIndex_ = histories_.size() - 1;

        intptr_t id = monapi_file_open(historyPath_.c_str(), FILE_CREATE);
        if (id < M_OK) {
            monapi_warn_once("can't open a history to %s\n", historyPath_.c_str());
            return;
        }

        intptr_t size = monapi_file_get_file_size(id);
        monapi_file_seek(id, size, SEEK_SET);
        MonAPI::SharedMemory shm(text.size() + 1);
        if (shm.map() != M_OK) {
            monapi_warn("shm map error on addHistory");
            return;
        }
        memcpy(shm.data(), text.data(), text.size());
        shm.data()[text.size()] = '\n';
        int sizeWritten = monapi_file_write(id, shm, shm.size());
        if (sizeWritten != shm.size()) {
            monapi_warn("history file write failure");
            return;
        }
        monapi_file_close(id);
    }

    void loadHistoriesFromFile(const std::string& path)
    {
        MonAPI::scoped_ptr<MonAPI::SharedMemory> shm(monapi_file_read_all(path.c_str()));
        if (shm.get() == NULL) {
            monapi_warn("failed");
            return;
        }
        std::string content((const char*)shm->data(), shm->size());
        histories_ = MonAPI::StringHelper::split("\n", content);
        historyIndex_ = histories_.size() - 1;
    }

    void showPreviousHistory()
    {
        if (historyIndex_ == 0) {
            return;
        }
        historyIndex_--;
        showHistory();
    }

    void showNextHistory()
    {
        if (historyIndex_ == (int)histories_.size() - 1) {
            return;
        }
        historyIndex_++;
        showHistory();
    }

    void showHistory()
    {
        ASSERT(historyIndex_ >= 0);
        ASSERT(historyIndex_ < (int)histories_.size());
        setText(histories_[historyIndex_].c_str());
    }
public:
    TextFieldWithHistory(const std::string& historyPath) : TextField(), historyIndex_(0), historyPath_(historyPath)
    {
        loadHistoriesFromFile(historyPath);
    }


    virtual ~TextFieldWithHistory() {}

    virtual void processEvent(Event* event)
    {
        if (event->getType() == KeyEvent::KEY_PRESSED &&
            ((KeyEvent*)event)->getModifiers() == KeyEvent::VKEY_CTRL) {
            if (((KeyEvent*)event)->getKeycode() == 'p') {
                showPreviousHistory();
            } else if (((KeyEvent*)event)->getKeycode() == 'n') {
                showNextHistory();
            } else {
                TextField::processEvent(event);
            }
        } else if (event->getType() == KeyEvent::KEY_PRESSED &&
                   ((KeyEvent*)event)->getKeycode() == KeyEvent::VKEY_ENTER) {
            addToHistories(getText());
        } else {
            TextField::processEvent(event);
        }
    }
};

#endif // _TEXTFIELD_WITH_HISTORY_
