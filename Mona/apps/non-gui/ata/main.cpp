#include <monapi.h>
#include "IDEDriver.h"

using namespace MonAPI;

int MonaMain(List<char*>* pekoe)
{
    syscall_get_io();

    IDEDriver ide;

    int controller;
    int device;
    int type;

    for (controller = 0; controller < 2; controller++)
    {
        for (device = 0; device < 2; device++)
        {
            type = ide.getDeviceType(controller, device);
            if (type == IDEDriver::DEVICE_ATAPI) break;
        }
    }

    if (type != IDEDriver::DEVICE_ATAPI)
    {
        printf("primary master device is not ATA\n");
        return 1;
    }

    ide.setDevice(controller, device);

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
