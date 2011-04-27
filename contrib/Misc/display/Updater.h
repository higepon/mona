/*
 * Updater.h -
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

#ifndef _UPDATER_
#define _UPDATER_

class Updater
{
public:
    void run()
    {
        for (MessageInfo msg;;)
        {
            if (MonAPI::Message::receive(&msg)) continue;
            switch (msg.header)
            {
            case MSG_UPDATE:
            {
                uint64_t s = MonAPI::Date::nowInMsec();
                intptr_t ret = update();
                uint64_t e = MonAPI::Date::nowInMsec();
                logprintf("update %d msec\n", (int)(e - s));
                if (ret != M_OK) {
                    monapi_warn("mosh abnormal exit");
                }
                ret = MonAPI::Message::send(msg.from, MSG_OK, ret);
                if (ret != M_OK) {
                    monapi_fatal("MSG_UPDATE send failed");
                }
                break;
            }
            default:
                monapi_warn("unknown message %d\n", msg.header);
                break;
            }
        }
    }
private:
    intptr_t update()
    {
        std::string command(MonAPI::System::getMoshPath());
        command += " /LIBS/MOSH/bin/fb-feed-get.sps";
        uint32_t tid;
        intptr_t result = monapi_call_process_execute_file_get_tid(command.c_str(),
                                                                   MONAPI_TRUE,
                                                                   &tid,
                                                                   MonAPI::System::getProcessStdinID(),
                                                                   MonAPI::System::getProcessStdoutID());
        if (result != M_OK) {
            monapi_warn("can't exec Mosh");
            return result;
        }
        return monapi_process_wait_terminated(tid);
    }
};

#endif // _UPDATER_
