#include <monapi.h>
#include <sys/HashMap.h>

using namespace MonAPI;

#define MAIN_3

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

   byte index;
   bool found = FindDevice(0, VENDOR_ID_DEC, DEVICE_ID_DEC_DC21140, &index);

   printf("DEC 21140 search...[%s]\n", found ? "OK" : "NG");

   if (!found) return 1;

   byte irq = ReadConfig(0, index, 0, PCI_IRQ_LINE, 1);

   printf("irq=%d\n", irq);
   printf("io base1=%x\n", ReadConfig(0, index, 0, PCI_BASE_ADDRESS1, 4));
   printf("io base2=%x\n", ReadConfig(0, index, 0, PCI_BASE_ADDRESS2, 4));
   printf("io base3=%x\n", ReadConfig(0, index, 0, PCI_BASE_ADDRESS3, 4));
   printf("io base4=%x\n", ReadConfig(0, index, 0, PCI_BASE_ADDRESS4, 4));
   printf("io base5=%x\n", ReadConfig(0, index, 0, PCI_BASE_ADDRESS5, 4));

   return 0;
}

#endif
