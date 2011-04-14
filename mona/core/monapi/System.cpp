#include <monapi.h>
#include <monapi/messages.h>

extern MonAPI::Stream* inStream;
extern MonAPI::Stream* outStream;

namespace MonAPI
{
    static char bundlePath[128];
    static char processPath[MESSAGE_INFO_MAX_STR_LENGTH];

    void System::getProcessInfo(PsInfo* dest)
    {
        uint32_t tid = getThreadID();
        syscall_set_ps_dump();
        PsInfo buf;
        while (syscall_read_ps_dump(&buf) == M_OK)
        {
            if (buf.tid == tid) {
                // Don't break here, which causes kernel memory leak.
                *dest = buf;
            }
        }
        return;
    }

    uint32_t System::getParentThreadID()
    {
        MessageInfo msg;
        uint32_t id ;
        if (monapi_name_whereis("/servers/process", id) != M_OK) {
            return NULL;
        }

        if (Message::sendReceive(&msg, id, MSG_PROCESS_GET_PROCESS_INFO) != M_OK)
        {
            return NULL;
        }
        return msg.arg2;
    }

    const char* System::getProcessPath()
    {
        uint32_t id ;
        if (monapi_name_whereis("/servers/process", id) != M_OK) {
            return NULL;
        }
        MessageInfo msg;
        if (Message::sendReceive(&msg, id, MSG_PROCESS_GET_PROCESS_INFO) != M_OK)
        {
            return NULL;
        }
        memcpy(processPath, msg.str, MESSAGE_INFO_MAX_STR_LENGTH);
        return processPath;
    }

    uint32_t System::getProcessStdoutID()
    {
        return System::getProcessStdoutID(getThreadID());
    }
    uint32_t System::getProcessStdoutID(uint32_t tid)
    {
        MessageInfo msg;
        uint32_t id ;
        if (monapi_name_whereis("/servers/process", id) != M_OK) {
            monapi_warn("/server/process not found");
            return NULL;
        }
        if (Message::sendReceive(&msg, id, MSG_PROCESS_GET_PROCESS_STDIO, tid) != M_OK)
        {
            monapi_warn("/server/process sendReceive failed");
            return NULL;
        }
        return msg.arg3;
    }
    uint32_t System::getProcessStdinID()
    {
        return System::getProcessStdinID(getThreadID());
    }
    uint32_t System::getProcessStdinID(uint32_t tid)
    {
        uint32_t id ;
        if (monapi_name_whereis("/servers/process", id) != M_OK) {
            return NULL;
        }
        MessageInfo msg;
        if (Message::sendReceive(&msg, id, MSG_PROCESS_GET_PROCESS_STDIO, tid) != M_OK)
        {
            return NULL;
        }
        return msg.arg2;
    }
    const char* System::getBundlePath()
    {
        const char* path = getProcessPath();
        char* app = strstr(path, ".APP/");
        if (app == NULL) return NULL;

        int len = app - path + 4;
        strncpy(bundlePath, path, len);
        bundlePath[len] = '\0';
        return bundlePath;
    }
    Stream* System::getStdinStream()
    {
        if (NULL != inStream) return inStream;
        uint32_t handle = System::getProcessStdinID();
        inStream = Stream::FromHandle(handle);
        return inStream;
    }
    Stream* System::getStdoutStream()
    {
        if (outStream == NULL) {
            uint32_t handle = System::getProcessStdoutID();
            if (handle == THREAD_UNKNOWN) {
                _logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
                outStream = NULL;
            } else {
                _logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
                outStream = Stream::FromHandle(handle);
            }
        }

        if (outStream == NULL) {
                _logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
            return NULL;
        } else if (outStream->getLastError() == M_OK) {
            return outStream;
        } else {
                _logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
            return NULL;
        }
    }
}
