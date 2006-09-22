//$Id$ 
#pragma once
#include <sys/types.h>
#include <monapi.h>
#include "Nic.h"

//VirtualPC 2004 acts as if she has 21140A.
//The author refers Intel 21143 PCI/CardBus 10/100Mb/s
//Ethernet LAN Controller Hardware Reference Manual Rev1.0 and others.

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
        RX_OWN=0x80000000,
    };
    void txihandler();
    byte* txbuf;
    DESC* txdsc;
    int txindex;
    int txdirty;
    enum{
        LOGTXRINGLEN=2,
        TX_OWN=0x80000000,
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
    void reset(){ outp32(iobase+CSR_0,CSR0_RESET); }
//    void stop(){ w_csr(CSR_CSR,CSR_STOP); disableNetwork(); };
    int ReadSROM(word,word*);
    void Delay800nSec(){ /*Be carefull when you use a real device.*/ };
    enum{ 
      CSR_0        =0x00,
      CSR0_RESET   =0x0001,
      CSR_2        =0x10,
      CSR_3        =0x18,
      CSR_4        =0x20,
      CSR_6        =0x30,
      CSR_8           =0x40,
      CSR_9        =0x48,
      CSR_12       =0x60,
      CSR_13       =0x68,
      CSR_14       =0x70,
      CSR_15       =0x78,

      //Following Constants were cited from 21041 App.Note.    
      MAC_OFFSET   =0x0A,
      SROM_SIZE    =0x3F,
      SROM_BITS    =0x05,
      CSR9_READ    =0x4000,
      CSR9_WRITE   =0x2000,
      SEL_SROM     =0x0800,
      DATA_1       =0x0004,
      DATA_0       =0x0000,
      CLK          =0x0002,
      CS           =0x0001,
      SETUPPKTSIZE =0xC0,
    };
    dword setupframe[SETUPPKTSIZE/4];
};
#pragma pack(pop)
};//mones::
