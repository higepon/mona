#include <monapi.h>
#include "IDEDriver.h"

using namespace MonAPI;

bool FindATAPIDevice(IDEDriver* ide, int* c, int* d)
{
    for (int controller = 0; controller < 2; controller++)
    {
        for (int device = 0; device < 2; device++)
        {
            int type = ide->getDeviceType(controller, device);
            if (type == IDEDriver::DEVICE_ATAPI)
            {
                *c = controller;
                *d = device;
                return true;
            }
        }
    }
    return false;
}

int MonaMain(List<char*>* pekoe)
{
    syscall_get_io();

    int controller;
    int device;
    IDEDriver ide;

    if (!FindATAPIDevice(&ide, &controller, &device))
    {
        printf("atapi device not found \n");
        return 1;
    }

#if 1
    ide.setDevice(controller, device);
#endif

#if 0
    ide.setDevice(0, 0);
#endif

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
