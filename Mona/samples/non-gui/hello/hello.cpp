#include <monapi.h>
#include <monapi/CString.h>
#include <sys/HashMap.h>
#include <monapi/Assert.h>
#include <monapi/messages.h>

using namespace MonAPI;

#define MAIN_1

#ifdef MAIN_8

int MonaMain(List<char*>* pekoe)
{
    byte* dma = monapi_allocate_dma_memory();

    printf("dma = %x\n", dma);

    dma[3] = 0xff;
    printf("after\n");
    monapi_deallocate_dma_memory(dma);

    dma[3] = 0xff;


    return 0;
}

#endif


#ifdef MAIN_7

int MonaMain(List<char*>* pekoe)
{
    if (pekoe->size() != 1)
    {
        printf("usage: hello filepath");
        return 1;
    }

    monapi_cmemoryinfo* mi = NULL;
    mi = monapi_call_file_read_data(pekoe->get(0), MONAPI_FALSE);

    if (mi == NULL)
    {
        printf("File not found\n");
        return 1;
    }

    for (int i = 0; i < mi->Size; i++)
    {
        printf("%c", mi->Data[i]);

    }

    monapi_cmemoryinfo_dispose(mi);
    monapi_cmemoryinfo_delete(mi);
    return 0;
}

#endif

#ifdef MAIN_1
int MonaMain(List<char*>* pekoe)
{
   printf("Hello World\n");
   return 0;
}
#endif

/* FileOutputStreamの実験 Process.cppの sendKilledMessageを有効にするとハング */
#ifdef MAIN_2
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

/* PCI列挙 */
#ifdef MAIN_3
typedef union
{
   dword command;
   struct
   {
       unsigned reserved1 : 2;
       unsigned reg       : 6;
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
   REG_CONFIG_DATA    = 0x0CFC,

   PCI_VENDOR_ID      = 0x00,
   PCI_DEVICE_ID      = 0x02,
   PCI_STATUS         = 0x06,
   PCI_REVISION       = 0x08,
   PCI_API            = 0x09,
   PCI_SUBCLASS       = 0x0a,
   PCI_BASECLASS      = 0x0b,
   PCI_HEADER         = 0x0e,
   PCI_BASE_ADDRESS1  = 0x10,
   PCI_BASE_ADDRESS2  = 0x14,
   PCI_BASE_ADDRESS3  = 0x18,
   PCI_BASE_ADDRESS4  = 0x1C,
   PCI_BASE_ADDRESS5  = 0x20,
   PCI_IRQ_LINE       = 0x3c
};

dword ReadConfig(byte bus, byte device, byte function, byte reg, byte readSize)
{
   dword result;
   PciPacket packet;

   packet.p.enabled   = 1;
   packet.p.bus       = bus;
   packet.p.device    = device;
   packet.p.function  = function;
   packet.p.reg       = reg & ~3;
   packet.p.reserved1 = 0;
   packet.p.reserved2 = 0;

   /* set request and enable */
   outp32(REG_CONFIG_ADDRESS, packet.command);

   switch (readSize)
   {
   case 1:
       result = inp8(REG_CONFIG_DATA + (reg & 3));
       break;

   case 2:
       result = inp16(REG_CONFIG_DATA + (reg & 3));
       break;

   case 4:
       result = inp32(REG_CONFIG_DATA + (reg & 3));
       break;

   default:
       result = 0xFFFFFFFF;
       break;
   }

   packet.p.enabled = 0;
   outp32(REG_CONFIG_ADDRESS, packet.command);
   return result;
}

bool FindDevice(byte bus, word vendor, word device, byte* deviceIndex)
{
   for (int i = 0; i < 32; i++)
   {
       word foundVendor = ReadConfig(0, i, 0, PCI_VENDOR_ID, 2);

       if (foundVendor == 0xFFFF || foundVendor != vendor) continue;

       word foundDevice = ReadConfig(0, i, 0, PCI_DEVICE_ID, 2);

       if (foundDevice != device) continue;

       *deviceIndex = i;
       return true;
   }
   return false;
}

enum
{
    VENDOR_ID_DEC         = 0x1011,
    DEVICE_ID_DEC_DC21140 = 0x0009,
    IRQ_NUMBER            = 0x0B,
    IO_BASE               = 0xEC00
};

int TuripProbe()
{


    return 0;
}

int MonaMain(List<char*>* pekoe)
{

   /* ユーザーモードI/O */
   syscall_get_io();

#if 0
   byte index;
   bool found = FindDevice(0, VENDOR_ID_DEC, DEVICE_ID_DEC_DC21140, &index);

   printf("DEC 21140 search...[%s]\n", found ? "OK" : "NG");

   if (!found) return 1;

   printf("revision=%d\n", ReadConfig(0, index, 0, PCI_REVISION, 1));
#endif

   /* とりあえず列挙でも */
   for (int i = 0; i < 32; i++)
   {
       word foundVendor = ReadConfig(0, i, 0, PCI_VENDOR_ID, 2);

       if (foundVendor == 0xFFFF) continue;

       printf("** PCI Device[%d] ** \n", i);
       printf("vendor = %x \n", foundVendor);
       printf("device = %x \n", ReadConfig(0, i, 0, PCI_DEVICE_ID, 2));

   }




   return 0;
}

#endif

#ifdef MAIN_4

#include "sys/BinaryTree.h"

int MonaMain(List<char*>* pekoe)
{
    BinaryTree<int> tree;

    tree.add(1000, 1);
    printf("%d\n", tree.get(1000));
    printf("%d\n", tree.get(1001));

    tree.add(1001, 2);
    printf("%d\n", tree.get(1000));
    printf("%d\n", tree.get(1001));


    printf("Hello World\n");
    return 0;
}
#endif

#ifdef MAIN_5

#define DEBUG_MODE

void write(const char* buf, int size)
{
    ASSERT(size < 127);

    MessageInfo msg;
    MessageInfo reply;
    Message::create(&msg, MSG_STDOUT, size, 0, 0, buf);

    if (Message::sendReceiveA(&reply, monapi_get_server_thread_id(ID_FILE_SERVER), &msg))
    {
        /* error, but nothing to do */
        ASSERT(!"stdout Error");

    }
}

void hoge()
{
    char hige[1024];
    memset(hige, 0, 1024);
    hoge();
    return;
}


int MonaMain(List<char*>* pekoe)
{
    hoge();

    dword filesize;
    printf("open=%d\n", syscall_file_open("hoge", 0, &filesize));

    CString msg = "this is test";

    write(msg, msg.getLength());

    return 0;
}


#endif

#ifdef MAIN_6


int MonaMain(List<char*>* pekoe)
{

    MemoryInfo info;

    syscall_get_memory_info(&info);
    printf("System Total Memory = %d (MB)\n", info.totalMemoryL / 1024 / 1024);

    printf("before malloc\n");
    printf("free pages %d / %d (KB) \n", info.freePageNum * info.pageSize / 1024, info.totalPageNum * info.pageSize / 1024);

    char* p = (char*)malloc(4096 * 10);

    syscall_get_memory_info(&info);
    printf("before free\n");
    printf("free pages %d / %d (KB) \n", info.freePageNum * info.pageSize / 1024, info.totalPageNum * info.pageSize / 1024);

    free(p);

    syscall_get_memory_info(&info);
    printf("after free\n");
    printf("free pages %d / %d (KB) \n", info.freePageNum * info.pageSize / 1024, info.totalPageNum * info.pageSize / 1024);

    return 0;
}
#endif
