//$Id$
#include "MonADEC.h"
using namespace MonAPI;
using namespace mones;

MonADEC::~MonADEC()
{
    
}

MonADEC::MonADEC()
{
    memcpy(devname,"dec21240a",10);
}

//Initalization Procedure is described in H.R.M 4.3.4
int MonADEC::init()
{    
    AllocateDmaPages( (1<<LOGRXRINGLEN)+ (1<<LOGTXRINGLEN));//pages.
    if( dma_head == NULL ){
        printf("buffer allocation was failed.");
        return -1;
    }
    reset();
//1. Wait 50 PCI clock cycles.
    sleep(100);
//2. Update configuration registers (Section 3.1): 
//3. Write CSR0 to set global host bus operating parameters (Section 3.2.2.1).
    outp32(iobase+CSR_0,0x01A08000);
//4. Write CSR7 to mask unnecessary (depending on the particular application) interrupt causes.
//5. The driver must create the transmit and receive descriptor lists. Then, it writes to both CSR3
//   and CSR4, providing the 21143 with the starting address of each list (Section 3.2.2.7). The
//   first descriptor on the transmit list may contain a setup frame (Section 4.2.3).
    rxdsc = (DESC*)dma_head;
    rxbuf = dma_head+((1<<LOGRXRINGLEN)*sizeof(DESC));
    for(int i=0;i<(1<<LOGRXRINGLEN);i++){
        (rxdsc+i)->status=0x80000000;//own
        (rxdsc+i)->ctlandcnt=ETHER_MAX_PACKET;
        (rxdsc+i)->bufaddr1=(dword)(rxbuf+i*ETHER_MAX_PACKET);
        (rxdsc+i)->bufaddr2=(dword)(rxdsc+i+1);
    }
    (rxdsc+((1<<LOGRXRINGLEN)-1))->bufaddr2=(dword)rxdsc;
    rxindex=0;
    rxdirty=0; 
    txdsc= (DESC*)( dma_head +(0x1000* (1<<LOGRXRINGLEN)));
    txbuf = dma_head + 0x1000*(1<<LOGRXRINGLEN) + (1<<LOGTXRINGLEN)*sizeof(DESC) ;
    for(int i=0;i<(1<<LOGTXRINGLEN);i++){
        (txdsc+i)->status=0;
        (txdsc+i)->ctlandcnt=ETHER_MAX_PACKET;
        (txdsc+i)->bufaddr1=(dword)(txbuf+i*ETHER_MAX_PACKET);
        (txdsc+i)->bufaddr2=(dword)(rxdsc+i+1);
    }
    (txdsc+((1<<LOGTXRINGLEN)-1))->bufaddr2=(dword)txdsc;
    txindex=0;
    txdirty=0;
    //setup frame must be set here?
    outp32(iobase+CSR_3,(dword)rxdsc);
    outp32(iobase+CSR_4,(dword)txdsc);
//6. When in either 10BASE-T or AUI mode, change the default settings of the jabber timers and
//   also the initial SIA settings by writing to CSR13 (Section 3.2.2.16), CSR14 (Section 3.2.2.17),
//   and CSR15 (Section 3.2.2.18).

//7. Write CSR6 (Section 3.2.2.9) to set global serial parameters and to start both the receive and
//   transmit processes. The receive and transmit processes enter the running state and attempt to
//   acquire descriptors from the respective descriptor lists. Then the receive and transmit
//   processes begin processing incoming and outgoing frames. The receive and transmit.

    printf("Hello Virtual PC\n");
    return 0;
}

int MonADEC::interrupt()
{
    printf("Interrupted\n");
    return 0;
}

void MonADEC::SendFrm(Ether* frame)
{

}
