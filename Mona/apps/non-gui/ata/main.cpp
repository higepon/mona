#include <monapi.h>
#include "IDEDriver.h"

using namespace MonAPI;

int MonaMain(List<char*>* pekoe)
{
    syscall_get_io();

    IDEDriver ide;

    ide.printDebug();

    int controller, deviceNo;

    if (!ide.findDevice(IDEDriver::DEVICE_ATAPI, 0x05, &controller, &deviceNo))
    {
        printf("CD-ROM Not Found\n");
        return 1;
    }

    if (!ide.selectDevice(controller, deviceNo))
    {
        printf("select device NG\n");
        return 1;
    }

    char buf[2048];
    memset(buf, 0, sizeof(buf));

    printf("read result = %d\n", ide.read(16, buf, 1024));

    FileOutputStream fos("HDDUMP.TXT");

    printf("fileout:open=%d\n", fos.open());

    printf("fileout:read=%d\n", fos.write((byte*)buf        , 512));
    printf("fileout:read=%d\n", fos.write((byte*)(buf + 512), 512));
    printf("fileout:read=%d\n", fos.write((byte*)buf  +1024      , 512));
    printf("fileout:read=%d\n", fos.write((byte*)(buf +1536), 512));

    fos.close();
    return 0;
}
