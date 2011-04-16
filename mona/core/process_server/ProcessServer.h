#ifndef __PROCESS_SERVER_H__
#define __PROCESS_SERVER_H__

#include <vector>
#include <monapi.h>
#include "ProcessInfo.h"
#include "ProcessExecuter.h"

class ProcessServer {
private:
    MonAPI::scoped_ptr<MonAPI::SharedMemory> commonParams_;
    std::vector<ProcessInfo> infos_;
    ProcessExecuter executer_;

public:
    ProcessServer() : commonParams_(new MonAPI::SharedMemory(sizeof(CommonParameters)))
    {
        if (commonParams_->map(false) != M_OK) {
            monapi_fatal("initCommonParameters failed");
        }
        if (monapi_notify_server_start("INIT") != M_OK) {
            monapi_fatal("monapi_notify_server_start to INIT failed");
        }

        if (monapi_name_add("/servers/process") != M_OK) {
            monapi_fatal("monapi_name_add failed");
        }
    }

private:

    void removeProcessInfo(uint32_t tid)
    {
        for (std::vector<ProcessInfo>::iterator it = infos_.begin(); it != infos_.end(); ++it) {
            if ((*it).tid == tid) {
                infos_.erase(it);
                return ;
            }
        }
        ASSERT(false);
    }

    intptr_t addProcessInfo(uint32_t parentTid, uint32_t subThreadTid)
    {
        ProcessInfo parent = getProcessInfo(parentTid);
        if (parent.tid == THREAD_UNKNOWN) {
            return M_NOT_FOUND;
        }
        infos_.push_back(ProcessInfo(subThreadTid, parent));
        return M_OK;
    }

    ProcessInfo getProcessInfo(uint32_t tid)
    {
        for (std::vector<ProcessInfo>::const_iterator it = infos_.begin(); it != infos_.end(); ++it) {
            if ((*it).tid == tid) {
                return *it;
            }
        }
        return ProcessInfo();
    }

    void addProcessInfo(uint32_t tid, uint32_t parent, const MonAPI::CString& name, const MonAPI::CString& path, uint32_t stdin_id, uint32_t stdout_id)
    {
        ProcessInfo pi(tid, parent, name, path, stdin_id, stdout_id);
        infos_.push_back(pi);
    }

    MonAPI::CString GetFileNameFromPath(const MonAPI::CString& path)
    {
        int p = path.lastIndexOf('/');
        if (p < 0) return path;
        p++;
        return path.substring(p, path.getLength() - p);
    }

    MonAPI::CString GetPathFromCommandLins(const MonAPI::CString& commandLine)
    {
        MonAPI::CString path;
        _A<MonAPI::CString> args = commandLine.split(' ');
        FOREACH (MonAPI::CString, arg, args)
        {
            if (arg == NULL) {
                continue;
            }
            if (path == NULL) {
                path = arg.toUpper();
                break;
            }
        }
        END_FOREACH
        return NULL;
    }

    int ExecuteFile(uint32_t parent, const MonAPI::CString& commandLine, uint32_t stdin_id, uint32_t stdout_id, uint32_t* tid, uint32_t observer)
    {
        intptr_t ret = executer_.ExecuteFile(parent, commandLine, stdin_id, stdout_id, tid, observer);
        if (ret == M_OK) {
            MonAPI::CString path = GetPathFromCommandLins(commandLine);
            addProcessInfo(*tid, parent, GetFileNameFromPath(path), path, stdin_id, stdout_id);
        }
        return ret;
    }

public:

    void service()
    {
        for (MessageInfo msg;;) {
            if (MonAPI::Message::receive(&msg) != 0) continue;

            switch (msg.header) {
            case MSG_PROCESS_EXECUTE_FILE:
            {
                uint32_t tid = 0;
                int result = ExecuteFile(msg.from, msg.str, msg.arg2, msg.arg3, &tid, msg.from);
                MonAPI::Message::reply(&msg, result, tid);
                break;
            }
            case MSG_PROCESS_GET_PROCESS_STDIO:
            {
                ProcessInfo pi = getProcessInfo(msg.arg1);
                MonAPI::Message::reply(&msg, pi.stdin_id, pi.stdout_id);
                break;
            }
            case MSG_PROCESS_TERMINATED:
                removeProcessInfo(msg.arg1);
                break;
            case MSG_PROCESS_GET_COMMON_PARAMS:
                MonAPI::Message::reply(&msg, commonParams_->handle());
                break;
            case MSG_PROCESS_REGISTER_THREAD:
            {
                uint32_t parentTid = msg.from;
                uint32_t subThreadTid = msg.arg1;
                intptr_t ret = addProcessInfo(parentTid, subThreadTid);
                MonAPI::Message::reply(&msg, ret);
                break;
            }
            }
        }
    }
};


#endif  // __PROCESS_SERVER_H__
