#include <monapi.h>
#include <monapi/CString.h>
#include <monapi/messages.h>
#include <monalibc.h>
#include "ELFServer.h"
#include "elfparser.h"

using namespace MonAPI;

static int CreateImage(SharedMemory** dest, uint32_t* entryPoint, SharedMemory* shm, bool prompt)
{
    ELFParser parser;
    if (!parser.set(shm->data(), shm->size()))
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

    SharedMemory* dst = new SharedMemory(parser.getImageSize());
    if (dst->map(true) != M_OK) {
        delete dst;
        return 3;
    }
    if (!parser.load(dst->data()))
    {
        if (prompt) _printf("%s: load failed!\n", SVR);
        delete dst;
        return 3;
    }

    *dest = dst;
    *entryPoint = parser.getEntryPoint();
    return 0;
}

static int CreateImage(SharedMemory** dest, uint32_t* entryPoint, const CString& path, bool prompt)
{
    SharedMemory* shm = NULL;
    if (path.endsWith(".EL2"))
    {
        shm = monapi_call_file_decompress_bz2_file(path, prompt ? MONAPI_TRUE : MONAPI_FALSE);
    }
    else if (path.endsWith(".EL5"))
    {
        shm = monapi_call_file_decompress_st5_file(path, prompt ? MONAPI_TRUE : MONAPI_FALSE);
    }
    else
    {
        shm = monapi_file_read_all(path);
    }
    if (shm == NULL)
    {
        return 1;
    }
    SharedMemory* img;
    int result = CreateImage(&img, entryPoint, shm, prompt);
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
            case MSG_PROCESS_CREATE_IMAGE:
            {
                SharedMemory* shm = NULL;
                uint32_t entryPoint = 0;
                int result = CreateImage(&shm, &entryPoint, msg.str, msg.arg1 == MONAPI_TRUE);
                if (result == 0)
                {
                    char buf[16];
                    sprintf(buf, "%d", shm->size());

                    // To prevent miss freeing of shared map, waits the client notification.
                    int ret = Message::sendReceive(&msg, msg.from, MSG_OK, msg.header, shm->handle(), entryPoint, buf);
                    delete shm;
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
    if (monapi_notify_server_start("MONITOR.BIN") != M_OK)
    {
        exit(-1);
    }

    MessageLoop();

    return 0;
}
