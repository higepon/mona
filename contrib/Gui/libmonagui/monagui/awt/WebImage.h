/*
 * WebImage.h
 *
 *   Depends on Mosh and http-get.sps
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
 *  $Id: WebImage.h 261 2008-07-25 06:16:44Z higepon $
 */

#ifndef _MONAGUI_WEBIMAGE_
#define _MONAGUI_WEBIMAGE_

namespace monagui {

class WebImage : public Image
{
public:
    WebImage()
    {
    }

    bool initialize(const std::string& uri, const std::string& path)
    {
        logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
        disposeImage();
        logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
        if (!monapi_file_exists(path.c_str())) {
        logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
            uint32_t tid;
            std::string cmd("/APPS/MOSH.APP/MOSH.EXE --loadpath=/LIBS/MOSH/lib /LIBS/MOSH/bin/http-get.sps");
            cmd += + ' ';
            cmd += uri;
            cmd += ' ';
            cmd += path;
            intptr_t result = monapi_process_execute_file_get_tid(cmd.c_str(),
                                                                  MONAPI_TRUE,
                                                                  &tid,
                                                                  MonAPI::System::getProcessStdinID(),
                                                                  MonAPI::System::getProcessStdoutID());
            if (result != 0) {
                monapi_fatal("can't exec Mosh");
                return false;
            }
            monapi_process_wait_terminated(tid);
        }
        logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
        initFromFilePath(path.c_str());
        logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
        return true;
    }
};
};

#endif // _MONAGUI_WEBIMAGE_
