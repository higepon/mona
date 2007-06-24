#include <monapi.h>
#include <monapi/CString.h>
#include <monapi/messages.h>
#include <monalibc.h>
#include "ELFServer.h"
#include "elfparser.h"

using namespace MonAPI;

static int CreateImage(monapi_cmemoryinfo** dest, uint32_t* entryPoint, monapi_cmemoryinfo* mi, bool prompt)
{
    ELFParser parser;
    if (!parser.set(mi->Data, mi->Size))
    {
        if (prompt) _printf("%s: file type is not ELF!\n", SVR);
        return 3;
    }

    int type = parser.getType();
    if (type != ELFParser::TYPE_RELOCATABLE && type != ELFParser::TYPE_EXECUTABLE)
    {
        if (prompt) _printf("%s: file type is not supported!\n", SVR);
        return 3;
    }

    int result = parser.parse();
    if (result != 0)
    {
        if (prompt) _printf("%s: can not parse!\n", SVR);
        return 3;
    }

    monapi_cmemoryinfo* dst = monapi_cmemoryinfo_new();
    if (!monapi_cmemoryinfo_create(dst, parser.getImageSize(), prompt ? MONAPI_TRUE : MONAPI_FALSE))
    {
        monapi_cmemoryinfo_delete(dst);
        return 3;
    }

    if (!parser.load(dst->Data))
    {
        if (prompt) _printf("%s: load failed!\n", SVR);
        monapi_cmemoryinfo_delete(dst);
        return 3;
    }

    *dest = dst;
    *entryPoint = parser.getEntryPoint();
    return 0;
}

static int CreateImage(monapi_cmemoryinfo** dest, uint32_t* entryPoint, const CString& path, bool prompt)
{
    monapi_cmemoryinfo* mi = NULL;
    if (path.endsWith(".EL2"))
    {
        mi = monapi_call_file_decompress_bz2_file(path, prompt ? MONAPI_TRUE : MONAPI_FALSE);
    }
    else if (path.endsWith(".EL5"))
    {
        mi = monapi_call_file_decompress_st5_file(path, prompt ? MONAPI_TRUE : MONAPI_FALSE);
    }
    else
    {
        mi = monapi_file_read_all(path);
    }
    if (mi == NULL)
    {
        return 1;
    }
    monapi_cmemoryinfo* img;
    int result = CreateImage(&img, entryPoint, mi, prompt);
    monapi_cmemoryinfo_dispose(mi);
    monapi_cmemoryinfo_delete(mi);
    if (result == 0) *dest = img;
    return result;
}

static void MessageLoop()
{
    for (MessageInfo msg;;)
    {
        if (Message::receive(&msg) != 0) continue;

        switch (msg.header)
        {
            case MSG_DISPOSE_HANDLE:
                MemoryMap::unmap(msg.arg1);
                Message::reply(&msg);
                break;
            case MSG_PROCESS_CREATE_IMAGE:
            {
                monapi_cmemoryinfo* mi = NULL;
                uint32_t entryPoint = 0;
                int result = CreateImage(&mi, &entryPoint, msg.str, msg.arg1 == MONAPI_TRUE);
                if (result == 0)
                {
                    char buf[16];
                    sprintf(buf, "%d", mi->Size);
                    Message::reply(&msg, mi->Handle, entryPoint, buf);
                    monapi_cmemoryinfo_delete(mi);
                }
                else
                {
                    Message::reply(&msg, 0, result);
                }
                break;
            }
            default:
                break;
        }
    }
}

int main(int argc, char* argv[])
{
    if (MONAPI_FALSE == monapi_notify_server_start("MONITOR.BIN"))
    {
        exit(-1);
    }

    MessageLoop();

    return 0;
}
