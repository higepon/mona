#include <monapi.h>

int MonaMain(List<char*>* pekoe)
{
    printf("Hello,World!\n");

#if 0
    dword fileID = monapi_file_open("/AUTOEXEC.MSH", MONAPI_FALSE);
    monapi_cmemoryinfo* mi = monapi_file_read(fileID, 486);
    printf("AUTOEXEC.MSH size = 485, readResult size = %d\n", mi->Size);
    monapi_file_close(fileID);
    monapi_cmemoryinfo_dispose(mi);
    monapi_cmemoryinfo_delete(mi);
#endif

#if 0
    const char* message = "Hello World\n";
    monapi_cmemoryinfo* buffer = new monapi_cmemoryinfo();
    if (!monapi_cmemoryinfo_create(buffer, strlen(message) + 1, 0))
    {
        monapi_cmemoryinfo_delete(buffer);
        return -1;
    }
    memcpy(buffer->Data, message, buffer->Size);
    dword id = monapi_file_open("/FD/HELLO.TXT", true);
    monapi_file_write(id, buffer, buffer->Size);
    monapi_file_close(id);
#endif
    return 0;
}
