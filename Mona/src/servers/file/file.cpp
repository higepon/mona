#include <monapi.h>
#include <monapi/messages.h>
#include "FileServer.h"
#include "file.h"

using namespace MonAPI;

monapi_cmemoryinfo* ReadFile(const char* file, bool prompt /*= false*/)
{
    if (prompt) printf("%s: Reading %s....", SVR, file);
    FileInputStream fis(file);
    if (fis.open() != 0)
    {
        if (prompt) printf("ERROR\n");
        return NULL;
    }

    monapi_cmemoryinfo* ret = monapi_cmemoryinfo_new();
    if (!monapi_cmemoryinfo_create(ret, fis.getFileSize() + 1, prompt))
    {
        delete ret;
        return NULL;
    }

    ret->Size--;
    fis.read(ret->Data, ret->Size);
    fis.close();
    ret->Data[ret->Size] = 0;
    if (prompt) printf("OK\n");
    return ret;
}
