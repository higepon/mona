#include <stdio.h>
#include "types.h"
#include "Message.h"
#include "servers/file.h"
#include "monapi/messages.h"

using namespace MonAPI;

int main()
{
    Message::initialize();

    monapi_cmemoryinfo* mi = monapi_call_file_read_directory("/", MONAPI_FALSE);
    monapi_directoryinfo* p = (monapi_directoryinfo*)&mi->Data[sizeof(int)];
    int size = *(int*)mi->Data;
    for (int i = 0; i < size; i++, p++)
    {
        printf("%s\n", p->name);
    }
    monapi_cmemoryinfo_dispose(mi);
    monapi_cmemoryinfo_delete(mi);

    return 0;
}
