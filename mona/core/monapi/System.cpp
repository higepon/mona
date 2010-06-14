#include <monapi.h>
#include <monapi/messages.h>

extern MonAPI::Stream* inStream;
extern MonAPI::Stream* outStream;

namespace MonAPI
{
    static PsInfo psInfo;
    static char bundlePath[128];
    static char processPath[MESSAGE_INFO_MAX_STR_LENGTH];

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
        if (Message::sendReceive(&msg, monapi_get_server_thread_id(ID_PROCESS_SERVER), MSG_PROCESS_GET_PROCESS_INFO) != M_OK)
        {
            return NULL;
        }
        return msg.arg2;
    }

    const char* System::getProcessPath()
    {
        MessageInfo msg;
        if (Message::sendReceive(&msg, monapi_get_server_thread_id(ID_PROCESS_SERVER), MSG_PROCESS_GET_PROCESS_INFO) != M_OK)
        {
            return NULL;
        }
        memcpy(processPath, msg.str, MESSAGE_INFO_MAX_STR_LENGTH);
        return processPath;
    }

    uint32_t System::getProcessStdoutID()
    {
        MessageInfo msg;
        if (Message::sendReceive(&msg, monapi_get_server_thread_id(ID_PROCESS_SERVER), MSG_PROCESS_GET_PROCESS_STDIO) != M_OK)
        {
            return NULL;
        }
        _logprintf("received stdouid=%x", msg.arg3);
        return msg.arg3;
    }
    uint32_t System::getProcessStdinID()
    {
        MessageInfo msg;
        if (Message::sendReceive(&msg, monapi_get_server_thread_id(ID_PROCESS_SERVER), MSG_PROCESS_GET_PROCESS_STDIO) != M_OK)
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
        _logprintf("%s:%d %s\n", __FILE__, __LINE__, System::getProcessInfo()->name);
        inStream = Stream::FromHandle(handle);
        return inStream;
    }
    Stream* System::getStdoutStream()
    {
        if (outStream == NULL) {
            uint32_t handle = System::getProcessStdoutID();
            if (handle == THREAD_UNKNOWN) {
                outStream = NULL;
            } else {
                outStream = Stream::FromHandle(handle);
            }
        }

        if (outStream == NULL) {
            _logprintf("%s:%d\n", __FILE__, __LINE__);

            return NULL;
        } else if (outStream->getLastError() == M_OK) {
            _logprintf("%s:%d\n", __FILE__, __LINE__);
            return outStream;
        } else {
            _logprintf("%s:%d %s\n", __FILE__, __LINE__, System::getProcessInfo()->name);
            return NULL;
        }
    }
}
