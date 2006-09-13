//$Id$ 
#pragma once
#include <sys/types.h>
#include <monapi.h>
#include "Nic.h"

//The author refers Intel 21143 PCI/CardBus 10/100Mb/s
//Ethernet LAN Controller Hardware Reference Manual Rev1.0 and others.
//VirtualPC acts as if she has 21140A.

namespace mones{ 

#pragma pack(push,4)
class MonADEC: public Nic
{
    //See H.R.M. (4.1)Figure4-2,(4.2)Figure4-7
    typedef struct DESC{
        word status;
        word ctlandcnt;
        word bufaddr1;
        word bufaddr2;
    };
protected:
    void rxihandler();
    byte* rxbuf;
    DESC* rxdsc;
    int rxindex;
    int rxdirty;
    enum{
        LOGRXRINGLEN=2,
        RMD1_OWN=0x8000,
        RMD1_STP=0x0200,
        RMD1_ENP=0x0100,
    };
    void txihandler();
    byte* txbuf;
    DESC* txdsc;
    int txindex;
    int txdirty;
    enum{
        LOGTXRINGLEN=2,
        TMD1_OWN=0x8000,
        TMD1_STP=0x0200,
        TMD1_ENP=0x0100, 
    };
public:    
    int   init();
    void  SendFrm(Ether*);
    int   interrupt();
    MonADEC();
    ~MonADEC();
    enum{
        VENDORID   =0x1011,
        DEVICEID   =0x0009, //21140A
    };
private:
//    word r_rap(){ return inp16(iobase+IO_RAP);}
 //   void w_rap(int reg){ outp16(iobase+IO_RAP,reg); }
  //  word r_csr(int reg){ w_rap(reg); return inp16(iobase+IO_RDP); }
  //  void w_csr(int reg,word val){ w_rap(reg); outp16(iobase+IO_RDP,val); }
  //  word r_bcr(int reg){ w_rap(reg); return inp16(iobase+IO_BDP); }
  //  void w_bcr(int reg,word val){ w_rap(reg); outp16(iobase+IO_BDP,val); }
    void reset(){ outp32(iobase+CSR_0,CSR_RESET); }
//    void stop(){ w_csr(CSR_CSR,CSR_STOP); disableNetwork(); };
    enum{ 
      CSR_0        =0x00,
      CSR_RESET    =0x00000001,
      CSR_2        =0x10,
      CSR_3           =0x18,
      CSR_4           =0x20,
      CSR_6           =0x30,
/*    IO_RDP       =0x10,
      IO_RAP       =0x12,
      IO_BDP       =0x16,
      BCR_MISC     =0x02,
      BCR_SSTYLE   =0x14,
    
      BCR_AUTOSEL  =0x0002, 
      BCR_EDGETRG  =0x0040,

      BCR_SSIZE    =0x0100,
      BCR_PCI_II   =0x0003,

      CSR_CSR      =0x00,
      CSR_IADR0    =0x01,
      CSR_IADR1    =0x02,
      CSR_FEATURE  =0x04,
      CSR_MAR0     =0x08,
      CSR_MAR1     =0x09,
      CSR_MAR2     =0x0A,
      CSR_MAR3     =0x0B,
      CSR_PADR0    =0x0C,
      CSR_PADR1    =0x0D,
      CSR_PADR2    =0x0E,
      CSR_MODE     =0x0F,
      CSR_RXADDR0  =0x18,
      CSR_RXADDR1  =0x19,
      CSR_TXADDR0  =0x1E,
      CSR_TXADDR1  =0x1F,
      CSR_POLLINT  =0x2F,
      CSR_RXRINGLEN=0x4C,
      CSR_TXRINGLEN=0x4E,

      CSR_INIT     =0x0001,
      CSR_START    =0x0002,
      CSR_STOP     =0x0004,
      CSR_TDMD     =0x0008,
      CSR_INTEN    =0x0040,
      CSR_INTR     =0x0080,
      CSR_TINT     =0x0200,
      CSR_RINT     =0x0400,
    
      FEAT_PADTX   =0x0800,
      FEAT_TXMSK   =0x0001,

      MODE_RXD     =0x0001,
      MODE_TXD     =0x0002,
      MODE_DNY_BCST=0x4000,
      MODE_PROMISC =0x8000,
      MODE_PSEL    =0x0180,
  */
    };
};
#pragma pack(pop)
};//mones::
