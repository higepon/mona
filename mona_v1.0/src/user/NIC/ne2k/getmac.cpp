/*!
    \file   getmac.cpp
    \brief  get and echo mac address for ne2k

    Copyright (c) 2004 gamix
    All rights reserved.
    License=MIT/X License

*/

#include <userlib.h>

// NIC io address
#define IO_ADDR 0x240

// NE2K Register
#define CR 0x00
// page 0 Read
#define CLD_A0  0x01
#define CLD_A1  0x02
#define BNRY    0x03
#define TSR     0x04
#define NCR     0x05
#define FIFO    0x06
#define ISR     0x07
#define CRDA_A0 0x08
#define CRDA_A1 0x09
#define _8019ID0 0x0A
#define _8019ID1 0x0B
#define RSR     0x0C
#define CNT_R0  0x0D
#define CNT_R1  0x0E
#define CNT_R2  0x0F
#define RMT_DMA 0x10
// page 0 Write
#define PSTART  0x01
#define PSTOP   0x02
#define BNRY   0x03
#define TPSR    0x04
#define TBC_R0  0x05
#define TBC_R1  0x06
#define ISR     0x07
#define RSA_R0  0x08
#define RSA_R1  0x09
#define RBC_R0  0x0A
#define RBC_R1  0x0B
#define RCR     0x0C
#define TCR     0x0D
#define DCR     0x0E
#define IMR     0x0F

// page 1 Read and Write
#define PAR0   0x01
#define PAR1   0x02
#define PAR2   0x03
#define PAR3   0x04
#define PAR4   0x05
#define PAR5   0x06
#define CURR   0x07
#define MAR0   0x08
#define MAR1   0x09
#define MAR2   0x0A
#define MAR3   0x0B
#define MAR4   0x0C
#define MAR5   0x0D
#define MAR6   0x0E
#define MAR7   0x0F

// page 2 Read
#define PSTART  0x01
#define PSTOP   0x02
#define TPSR    0x04
#define RCR     0x0C
#define TCR     0x0D
#define DCR     0x0E
#define IMR     0x0F

// conf reg
#define NE2K_PAGE0 0
#define NE2K_PAGE1 0x40
#define NE2K_PAGE2 0x80
#define NE2K_PAGE3 0xd0
#define NE2K_DMA_DISABLE 0x00
#define NE2K_DMA_READ 0x08
#define NE2K_DMA_WRITE 0x10
#define NE2K_DMA_SEND 0x18
#define NE2K_DMA_ABORT 0x20
// TXP is Tx Pakcet
#define NE2K_TXP 0x04
//STA bit is non used in RTL8019.
#define NE2K_STA 0x02
// STP is STOP NIC
#define NE2K_STP 0x01

//ISR Interrupt Status Regster
/*
#define RST 0x80
#define RDC 0x40
#define CNT 0x20
#define TXE 0x10
#define RXE 0x08
#define PTX 0x04
##define PTX 0x02
#define PRX 0x01
*/
//IMR Interrupt Musk Register
#define RDCE 0x40
#define CNTE 0x20
#define OVWE 0x10
#define TXEE 0x08
#define RXEE 0x04
#define PTXE 0x02
#define PRXE 0x01

//DCR Data Conf Reg
#define DCR_P 0x80
#define FT11 0x60
#define FT10 0x40
#define FT01 0x20
#define ARM 0x10
#define LS 0x08
#define BOS86 0
#define BOS68 0x02
//Word Transfer Register
#define WTS 0x01

//TCR
#define TCR_P 0xE0
#define OFST 0x10
#define ATD 0x08
#define NE2K_LB_NORMAL 0
#define NE2K_LB_INTERNAL 0x02
#define NE2K_LB_EXTERNAL 0x04
#define NE2K_GEN_CRC 0x00
#define NE2K_NOT_GEN_CRC 0x01


#define NE2K_RESET  0x1F

unsigned char hextoa(int hex)
{
   if ((hex >  16) || (hex < 0)) {
      printf("input error :input is %x\n",hex);
   } else if (10 <= hex) {
      return (hex - 10 + 'A');
   } else  {
     return (hex+'0');
   }
}

void char_to_MAC_string(unsigned char *from,unsigned char *to)
{
   for (int i=0;i < 6; i++){
       int ue=0;
       int shita=0;

       shita = *from & 0xf;
       ue = (*from & 0xf0) >> 4;

       *to=hextoa(ue);
       *(++to)=hextoa(shita);
       if(i < 5)
           *(++to)=':';
       to++;
       from=from+2;
   }
}

int MonaMain(List<char*>* pekoe) {

    int i=0;
    int port=0;
    unsigned char nicmem[20]={0};
    unsigned char macstr[20]={0};
    unsigned char *buf=nicmem;
    syscall_get_io();

    port = IO_ADDR;


    outp8(port,NE2K_PAGE0 | NE2K_DMA_ABORT | NE2K_STP);
    outp8(port+DCR,FT10 | BOS86 | LS  );
    outp8(port,NE2K_PAGE0 | NE2K_DMA_ABORT | NE2K_STA);
    outp8(port+RBC_R0,16);
    outp8(port+RBC_R1,0);
    outp8(port+RSA_R0,0);
    outp8(port+RSA_R1,0);
    outp8(port,NE2K_PAGE0 | NE2K_DMA_READ | NE2K_STA);

    for (i=0;i< 16 ;i=i++){
        *buf++=inp8( port+RMT_DMA);
    }

    char_to_MAC_string(nicmem,macstr);
    printf("MAC ADDRESS = %s",macstr);

    return 0;
}
