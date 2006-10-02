#include <monapi.h>
#include <monalibc.h>

using namespace MonAPI;

dword getStreamHandle()
{
    dword id = Message::lookupMainThread("STDOUT.EX5");
    if (THREAD_UNKNOWN == id)
    {
        printf("STDOUT.EX5 not found\n");
        exit(-1);
    }
    MessageInfo msg;
    Message::sendReceive(&msg, id, MSG_STREAM_ID);
    return msg.arg2;
}


int MonaMain(List<char*>* pekoe)
{
    if (pekoe->size() < 1)
    {
        printf("usage : WRITE text\n");
        return 0;
    }
    dword handle = getStreamHandle();
    Stream* stream = Stream::FromHandle(handle);
    char* text = pekoe->get(0);
    dword length = strlen(text);
    while (length > 0)
    {
        byte buffer[64];
        dword size = length >= 64 ? 64 : length;
        memcpy(buffer, (byte*)((dword)text + (strlen(text) - length)), size);
        stream->waitForWrite();
        dword writeSize = stream->write(buffer, size);
        length -= writeSize;
    }
    delete stream;
}
