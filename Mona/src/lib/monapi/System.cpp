#include <monapi.h>

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
    
    const char* System::getProcessPath()
    {
        return getProcessInfo()->path;
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
