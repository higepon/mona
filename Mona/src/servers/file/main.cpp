#include <monapi.h>
#include <monapi/messages.h>
#include <monapi/cmemoryinfo.h>

#define SVR "File Server"

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
    if (!monapi_cmemoryinfo_create(ret, fis.getFileSize(), prompt))
    {
        delete ret;
        return NULL;
    }

    fis.read(ret->Data, ret->Size);
    fis.close();
    if (prompt) printf("OK\n");
    return ret;
}

void MessageLoop()
{
    for (MessageInfo msg;;)
    {
        if (Message::receive(&msg)) continue;

        switch (msg.header)
        {
            case MSG_FILE_READ_DATA:
            {
                monapi_cmemoryinfo* mi = ReadFile(msg.str, msg.arg1);
                if (mi != NULL)
                {
                    Message::send(msg.from, MSG_RESULT_OK, msg.header, mi->Handle, mi->Size);
                    monapi_cmemoryinfo_delete(mi);
                }
                else
                {
                    Message::send(msg.from, MSG_RESULT_OK, msg.header);
                }
                break;
            }
            case MSG_DISPOSE_HANDLE:
                MemoryMap::unmap(msg.arg1);
                break;
        }
    }
}

int MonaMain(List<char*>* pekoe)
{
    if (Message::send(Message::lookupMainThread("INIT"), MSG_SERVER_START_OK) != 0)
    {
        printf("%s: INIT error\n", SVR);
        exit(1);
    }

    MessageLoop();

    return 0;
}
