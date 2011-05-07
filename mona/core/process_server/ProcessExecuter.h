/*
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

#ifndef _PROCESS_EXECUTER_
#define _PROCESS_EXECUTER_

class ProcessExecuter {
    int ExecuteProcess(uint32_t parent, MonAPI::SharedMemory& shm, uint32_t entryPoint, const MonAPI::CString& path, const MonAPI::CString& name, CommandOption* option, bool prompt, uint32_t stdin_id, uint32_t stdout_id, uint32_t* tid, uint32_t observer)
        {
            LoadProcessInfo info;
            info.image = shm.data();
            info.size  = shm.size();
            info.entrypoint = entryPoint;
            info.name = name;
            info.list = option;
            info.observer = observer;

            intptr_t ret = syscall_load_process_image(&info);
            if (ret != M_OK) {
                monapi_warn("failed to execute process %s", (const char*)path);
            }
            *tid = info.tid;

            return ret;
        }

public:
    MonAPI::CString GetFileName(const MonAPI::CString& path)
        {
            int p = path.lastIndexOf('/');
            if (p < 0) return path;

            p++;
            return path.substring(p, path.getLength() - p);
        }

    int ExecuteFile(uint32_t parent, const MonAPI::CString& commandLine, uint32_t stdin_id, uint32_t stdout_id, uint32_t* tid, uint32_t observer)
        {
            uint64_t s1 = MonAPI::Date::nowInMsec();
            bool prompt = false; /* todo */
            /* list initilize */
            CommandOption list;
            list.next = NULL;
            CommandOption* option = NULL;
            MonAPI::CString path;
            _A<MonAPI::CString> args = commandLine.split(' ');
            FOREACH (MonAPI::CString, arg, args)
            {
                if (arg == NULL) continue;

                if (path == NULL)
                {
                    path = arg.toUpper();
                    continue;
                }
                option = new CommandOption;
                strncpy(option->str, arg, sizeof(option->str));
                option->next = list.next;
                list.next = option;
            }
            END_FOREACH
            uint64_t s2 = MonAPI::Date::nowInMsec();
            MonAPI::SharedMemory* shm = NULL;
            uint32_t entryPoint = 0xa0000000;
            int result = 1;
            const char* svr_id = NULL;
            if (path.endsWith(".ELF") || path.endsWith(".EL2") || path.endsWith(".EL5"))
            {
                svr_id = "/servers/elf";
            }
            else if (path.endsWith(".EXE") || path.endsWith(".EX2") || path.endsWith(".EX5"))
            {
                svr_id = "/servers/pe";
            }
            if (svr_id != NULL)
            {
                MessageInfo msg;
                uint32_t tid;
                if (monapi_name_whereis(svr_id, tid) != M_OK) {
                    monapi_fatal("server not found");
                }
                uint64_t ss1 = MonAPI::Date::nowInMsec();

                if (MonAPI::Message::sendReceive(&msg, tid, MSG_PROCESS_CREATE_IMAGE, MONAPI_TRUE /* TODO */, 0, 0, path) != M_OK) {
                    _printf("Error %s:%d\n", __FILE__, __LINE__);
                    exit(-1);
                }
                uint64_t ss2 = MonAPI::Date::nowInMsec();
                _logprintf("ss2-ss1=%d\n", (int)(ss2 - ss1));
                if (msg.arg2 != 0) {
                    result = 0;
                    entryPoint = msg.arg3;
                    shm = new MonAPI::SharedMemory(msg.arg2, atoi(msg.str));
                    if (shm->map(true) != M_OK) {
                        monapi_fatal("Error %s:%d\n", __FILE__, __LINE__);
                        exit(-1);
                    }
                    // notify map is done.
                    int status = MonAPI::Message::reply(&msg);
                    if (status != M_OK) {
                        monapi_warn("%s reply failed : %s\n", __func__, monapi_error_string(status));
                    }

                }
                else
                {
                    result = msg.arg3;
                }
            }
            else if (path.endsWith(".BN2"))
            {
                shm = monapi_call_file_decompress_bz2_file(path, prompt ? MONAPI_TRUE : MONAPI_FALSE);
            }
            else if (path.endsWith(".BN5"))
            {
                shm = monapi_call_file_decompress_st5_file(path, prompt ? MONAPI_TRUE : MONAPI_FALSE);
            }
            else
            {
                shm = monapi_file_read_all(path);
            }
            uint64_t s3 = MonAPI::Date::nowInMsec();
            if (shm == NULL)
            {
                return result;
            }
            else
            {
                result = ExecuteProcess(parent, *shm, entryPoint, path, GetFileName(path), &list, prompt, stdin_id, stdout_id, tid, observer);
                delete shm;
            }
            CommandOption* next;
            for (option = list.next; option; option = next)
            {
                next = option->next;
                delete option;
            }
            uint64_t s4 = MonAPI::Date::nowInMsec();
            _logprintf("s4 - s3 = %d s3 - s2 = %d s2 - s1 = %d\n", (int)(s4 - s3), (int)(s3 - s2), (int)(s2 - s1));
            return result;
        }
};

#endif // _PROCESS_EXECUTER_
