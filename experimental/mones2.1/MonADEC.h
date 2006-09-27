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
private:
    //See H.R.M. (4.1)Figure4-2,(4.2)Figure4-7
    typedef struct DESC{
        dword status;
        dword ctlandcnt;
        dword bufaddr1;
        dword bufaddr2;
    };  
    enum{
        LOGRXRINGLEN=5,
        RX_OWN=0x80000000,
    };
    enum{
        LOGTXRINGLEN=6,
        TX_OWN=0x80000000,
    };
protected:
    void rxihandler(); 
    void txihandler();
    byte* rxbuf;  
    byte* txbuf;
    DESC* rxdsc;
    DESC* txdsc;
    int rxindex;  
    int txindex;  
    int   interrupt();
public:    
    int   init();
    void  SendFrm(Ether*);
    MonADEC();
    ~MonADEC();
    enum{
        VENDORID   =0x1011,
        DEVICEID   =0x0009, //21140A
    };
private:
    void reset(){ outp32(iobase+CSR_0,CSR0_RESET); }
    int ReadSROM(word,word*);
    void Delay800nSec(){ /*Be carefull when you use a real device.*/ };
    enum{ 
      CSR_0        =0x00,
      CSR0_RESET   =0x0001,
      CSR0_CA32    =0xA000,
      CSR_1        =0x08,
      CSR_2        =0x10,
      CSR_3        =0x18,
      CSR_4        =0x20,
      CSR_5        =0x28,
      CSR5_RI      =0x0040,
      CSR5_TI      =0x0001,
      CSR_6        =0x30,
      CSR_7        =0x38,
      CSR7_NI      =0x10000,
      CSR7_AI      =0x8000,
      CSR7_ERE     =0x4000,
      CSR7_FBE     =0x2000,
      CSR7_GPT     =0x0800,
      CSR7_ETE     =0x0400,
      CSR7_RW      =0x0200,
      CSR7_RS      =0x0100,
      CSR7_RU      =0x0080,
      CSR7_RI      =0x0040,
      CSR7_UN      =0x0020,
      CSR7_TJ      =0x0008,
      CSR7_TU      =0x0004,
      CSR7_TS      =0x0002,
      CSR7_TI      =0x0001,
      CSR_8        =0x40,
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
    //dword* setupframe;
};
#pragma pack(pop)
};//mones::
