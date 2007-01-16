#include <monapi.h>
#include <monapi/messages.h>

extern MonAPI::Stream* inStream;
extern MonAPI::Stream* outStream;

namespace MonAPI
{
    static PsInfo psInfo;
    static char bundlePath[128];

    PsInfo* System::getProcessInfo()
    {
        uint32_t tid = getThreadID();
        syscall_set_ps_dump();
        while (syscall_read_ps_dump(&psInfo) == 0)
        {
            if (psInfo.tid == tid) break;
        }
        return &psInfo;
    }

    uint32_t System::getParentThreadID()
    {
        MessageInfo msg;
        if (Message::sendReceive(&msg, monapi_get_server_thread_id(ID_PROCESS_SERVER), MSG_PROCESS_GET_PROCESS_INFO) != 0)
        {
            return NULL;
        }
        return msg.arg2;
    }

    const char* System::getProcessPath()
    {
        MessageInfo msg;
        if (Message::sendReceive(&msg, monapi_get_server_thread_id(ID_PROCESS_SERVER), MSG_PROCESS_GET_PROCESS_INFO) != 0)
        {
            return NULL;
        }
        return msg.str;
    }

    uint32_t System::getProcessStdoutID()
    {
        MessageInfo msg;
        if (Message::sendReceive(&msg, monapi_get_server_thread_id(ID_PROCESS_SERVER), MSG_PROCESS_GET_PROCESS_STDIO) != 0)
        {
            return NULL;
        }
        return msg.arg3;
    }
    uint32_t System::getProcessStdinID()
    {
        MessageInfo msg;
        if (Message::sendReceive(&msg, monapi_get_server_thread_id(ID_PROCESS_SERVER), MSG_PROCESS_GET_PROCESS_STDIO) != 0)
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
        if (NULL != outStream) return outStream;
        uint32_t handle = System::getProcessStdoutID();
        outStream = Stream::FromHandle(handle);
        return outStream;
    }
}
