#include <monapi.h>
#include <sys/HashMap.h>

using namespace MonAPI;

#if 1
int MonaMain(List<char*>* pekoe)
{
    printf("Hello World\n");
    return 0;
}
#endif

/* FileOutputStream‚ÌŽÀŒ± Process.cpp‚Ì sendKilledMessage‚ð—LŒø‚É‚·‚é‚Æƒnƒ“ƒO */
#if 0
int MonaMain(List<char*>* pekoe)
{
    int reti;

    char buf[1024];
    memset(buf, 0, sizeof(buf));

    sprintf(buf , "protType:%04x \n",1234);

    FileOutputStream fos("YTEST.TXT");

    printf("fileout:open=%d\n", fos.open());

    reti = fos.write((byte*)buf , 512);
    printf("fileout:read=%d\n", reti);

    printf("fos close pre\n");
    fos.close();
    printf("fos close after\n");

    return 0;
}
#endif

/* PCI—ñ‹“ */
#if 0
typedef union
{
    dword command;
    struct
    {
        unsigned reserved1 : 2;
        unsigned address   : 6;
        unsigned function  : 3;
        unsigned device    : 5;
        unsigned bus       : 8;
        unsigned reserved2 : 7;
        unsigned enabled   : 1;
    } p;
} PciPacket;

enum
{
    REG_CONFIG_ADDRESS = 0x0CF8,
    REG_CONFIG_DATA    = 0x0CFC


};

int MonaMain(List<char*>* pekoe)
{

    syscall_get_io();


    for (int i = 0; i < 32; i++)
    {
        PciPacket packet;
        dword in;

        packet.p.enabled = 1;
        packet.p.bus = 0;
        packet.p.device = i;
        packet.p.function = 0;
        packet.p.address = 0;
        packet.p.reserved1 = 0;
        packet.p.reserved2 = 0;

        outp32(REG_CONFIG_ADDRESS, packet.command);

        in = inp32(REG_CONFIG_DATA);

        printf("[%x:%x]\n", in & 0x0000FFFF, in >> 16);

        packet.p.enabled = 0;
        outp32(REG_CONFIG_ADDRESS, packet.command);
    }
}

#endif
