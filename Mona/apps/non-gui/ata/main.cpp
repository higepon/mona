#include <monapi.h>
#include "IDEDriver.h"

using namespace MonAPI;

int MonaMain(List<char*>* pekoe)
{
    /* ’¥æ’¡¼’¥¶’¡¼’¥â’¡¼’¥É’¤ÇI/O’¤·’¤Þ’¤¹’¤è’¡¼’¡£ */
    syscall_get_io();

    /* ’¥³’¥ó’¥¹’¥È’¥é’¥¯’¥¿’¤Ç’¤Ï’¥Ç’¥Õ’¥©’¥ë’¥È’¤ÇPrimary:Master’¤Î’¥Ç’¥Ð’¥¤’¥¹’¤¬’Áª’Âò’¤µ’¤ì’¤Æ’¤¤’¤Þ’¤¹’¤è’¡¼ */
    IDEDriver ide;

    /* ’°ì’±þ’¥Ç’¥Ð’¥¤’¥¹’¥Á’¥§’¥Ã’¥¯ */
    int type = ide.getDeviceType(IDEDriver::PRIMARY, IDEDriver::MASTER);

    if (type != IDEDriver::DEVICE_ATA)
    {
        printf("primary master device is not ATA\n");
        return 1;
    }

    char buf[1024];
    memset(buf, 0, sizeof(buf));

    /* lba 1’¤«’¤é1024byte’¤Þ’¤È’¤á’¤è’¤ß’¡¼’¡£512byte’¤¬1’Ã±’°Ì’¤Ç’¤¹ */
    printf("read result = %d", ide.read(1, buf, 1024));

    /* ’ÆÉ’¤ó’¤À’Æâ’ÍÆ’¤òFD’¤Ë’ÊÝ’Â¸ */
    FileOutputStream fos("/HDD.LOG");
    fos.open();

    fos.write((byte*)buf      , 512);
    fos.write((byte*)(buf + 512), 512);

    fos.close();

    return 0;
}
