#include <monapi.h>

using namespace MonAPI;

static Stream* stream;
dword id;

void output()
{
    for (;;)
    {
        byte buffer[32];
//        printf("wait for read before\n");
        stream->waitForRead();
//        printf("wait for read after\n");
        dword size = stream->read(buffer, 32);
//        printf("**** output start *** size = %d\n", size);
        for (dword i = 0; i < size; i++)
        {
            printf("%c", buffer[i]);
        }
        printf("\n");
    }
}

int MonaMain(List<char*>* pekoe)
{
    stream = new Stream(16);
    id = syscall_mthread_create((dword)output);
    syscall_mthread_join(id);
    for (MessageInfo msg; ;)
    {
        if (Message::receive(&msg)) continue;

        switch (msg.header)
        {
        case MSG_STREAM_ID:
            MonAPI::Message::reply(&msg, stream->handle());
            break;
        default:
            break;
        }
    }
    delete stream;
    return 0;
}
