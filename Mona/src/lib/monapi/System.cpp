#include <monapi.h>
#include <monapi/messages.h>

namespace MonAPI
{
    static PsInfo psInfo;
    static char bundlePath[128];

    PsInfo* System::getProcessInfo()
    {
        dword tid = getThreadID();
        syscall_set_ps_dump();
        while (syscall_read_ps_dump(&psInfo) == 0)
        {
            if (psInfo.tid == tid) break;
        }
        return &psInfo;
    }

    dword System::getParentThreadID()
    {
        MessageInfo msg;
        if (monapi_cmessage_send_receive_args(NULL, &msg, monapi_get_server_thread_id(ID_PROCESS_SERVER), MSG_PROCESS_GET_PROCESS_INFO, 0, 0, 0, NULL) != 0)
        {
            return NULL;
        }
        return msg.arg2;
    }

    const char* System::getProcessPath()
    {
        MessageInfo msg;
        if (monapi_cmessage_send_receive_args(NULL, &msg, monapi_get_server_thread_id(ID_PROCESS_SERVER), MSG_PROCESS_GET_PROCESS_INFO, 0, 0, 0, NULL) != 0)
        {
            return NULL;
        }
        return msg.str;
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
}
