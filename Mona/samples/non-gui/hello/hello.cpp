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
   PCI_HEADER         = 0x0e
};

dword ReadConfig(byte bus, byte device, byte function, byte reg, byte readSize)
{
   dword result;
   PciPacket packet;

   packet.p.enabled   = 1;
   packet.p.bus       = bus;
   packet.p.device    = device;
   packet.p.function  = function;
   packet.p.reg       = reg;
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

int MonaMain(List<char*>* pekoe)
{
   /* ユーザーモードI/O */
   syscall_get_io();

   /* bus0 のデバイス探索 */
   for (int i = 0; i < 32; i++)
   {
       word vendor = ReadConfig(0, i, 0, PCI_VENDOR_ID, 2);

       /* no device */
       if (vendor == 0xFFFF) continue;

       printf("**** device(%d)*****************************************************************************\n",i);
       printf("vendor    = %x  ", vendor);
       printf("device    = %x  ", ReadConfig(0, i, 0, PCI_DEVICE_ID, 2));
       printf("revision  = %x  ", ReadConfig(0, i, 0, PCI_REVISION , 1));
       printf("status    = %x\n", ReadConfig(0, i, 0, PCI_STATUS   , 2));
       printf("header    = %x  ", ReadConfig(0, i, 0, PCI_HEADER   , 1));
       printf("api       = %x  ", ReadConfig(0, i, 0, PCI_API      , 1));
       printf("subclass  = %x  ", ReadConfig(0, i, 0, PCI_SUBCLASS , 1));
       printf("baseclass = %x\n", ReadConfig(0, i, 0, PCI_BASECLASS, 1));
   }
   return 0;
}

#endif
