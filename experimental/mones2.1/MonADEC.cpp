//$Id$
#include "MonADEC.h"
using namespace MonAPI;
using namespace mones;

MonADEC::~MonADEC()
{
    
}

MonADEC::MonADEC()
{
    memcpy(devname,"dc21140a",9);
}

//Initalization Procedure is described in H.R.M 4.3.4
int MonADEC::init()
{    
    disableNetwork();
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
        (rxdsc+i)->status=RX_OWN;
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
    //Get serial rom contents.
    word val[SROM_SIZE];
    if( ReadSROM(SROM_SIZE,val) ){
        return -1;
    }
    //Create setup frame.
    for(int i=0;i<3;i++)
        setupframe[i]=0x0000ffff;
    for(int i=3;i<SETUPPKTSIZE/4;i++){
        setupframe[i]=val[MAC_OFFSET+(i%3)];
        //printf("%x:",setupframe[i]); 
        //MAC 0003 FF5A AB0E
        //see vmc file.
    }
    txdsc->status=TX_OWN;
    txdsc->ctlandcnt=0x08000000 | SETUPPKTSIZE;
    txdsc->bufaddr1=(dword)setupframe;
    txindex++;

    outp32(iobase+CSR_3,(dword)rxdsc);
    outp32(iobase+CSR_4,(dword)txdsc);
    //6. Write CSR6 (Section 3.2.2.6) to set global serial parameters and to start both the receive and
    //   transmit processes. The receive and transmit processes enter the running state and attempt to
    //   acquire descriptors from the respective descriptor lists. Then the receive and transmit
    //   processes begin processing incoming and outgoing frames. The receive and transmit.
    outp32( iobase + CSR_6,inp32(iobase + CSR_6) | 0x0202);

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
    printf("send frame\n");
}

int MonADEC::ReadSROM(word Len,word* Data)
{
    //This function was cited from 
    //"Using the Digital Semiconductor 
    // 21041 with BootROM,SerialROM and External Register,
    // An Application Note"
    // Appendix A  Serial ROM Access - Software Description.
    // Make sure the ROM_Address is not too big for this ROM
    if ( Len -1 > SROM_SIZE){
        printf("Data length is too big for SROM\n");
        return -1;
    }
    // Loop on all DATA words.
    for (word j=0; j<Len; j++){
        // Output the READ command to the SROM
        outp32(iobase+CSR_9, CSR9_WRITE | SEL_SROM | CS |       DATA_1);
        outp32(iobase+CSR_9, CSR9_WRITE | SEL_SROM | CS | CLK | DATA_1);
        Delay800nSec();
        outp32(iobase+CSR_9, CSR9_WRITE | SEL_SROM | CS |       DATA_1);
        Delay800nSec();
        outp32(iobase+CSR_9, CSR9_WRITE | SEL_SROM | CS |       DATA_1);
        outp32(iobase+CSR_9, CSR9_WRITE | SEL_SROM | CS | CLK | DATA_1);
        Delay800nSec();
        outp32(iobase+CSR_9, CSR9_WRITE | SEL_SROM | CS |       DATA_1);
        Delay800nSec();
        outp32(iobase+CSR_9, CSR9_WRITE | SEL_SROM | CS |       DATA_0);
        outp32(iobase+CSR_9, CSR9_WRITE | SEL_SROM | CS | CLK | DATA_0);
        Delay800nSec();
        outp32(iobase+CSR_9, CSR9_WRITE | SEL_SROM | CS |       DATA_0);
        Delay800nSec();
        // Output the WORD Address of the SROM
        for (word i=0; i<=SROM_BITS; i++){
            dword Dbit = (dword)(( j >> (SROM_BITS-i)) & 1) << 2;
            outp32(iobase+CSR_9, CSR9_WRITE | SEL_SROM | CS |       Dbit);
            outp32(iobase+CSR_9, CSR9_WRITE | SEL_SROM | CS | CLK | Dbit);
            Delay800nSec();    
            outp32(iobase+CSR_9, CSR9_WRITE | SEL_SROM | CS |       Dbit);
            Delay800nSec();
        }
        // Verify that the SROM output data became now 0.
        dword Dout=inp32(iobase+CSR_9);
        Dout = (Dout>>3) & 1;
        if (Dout != 0){
            printf("SROM did not become busy in read command\n");
            return -1;    
        }
        word WordData=0; // !!
        // Input the WORD of data from the SROM
        for (word i=0; i<=15; i++){
            outp32(iobase+CSR_9, CSR9_WRITE | SEL_SROM | CS | CLK);
            Delay800nSec();
            Dout=inp32(iobase+CSR_9);
            WordData |= ((Dout>>3) & 1) << (15-i);
            outp32(iobase+CSR_9, CSR9_WRITE | SEL_SROM | CS );
            Delay800nSec();
        }
        //Put our read data in user buffer
        *Data = WordData;
        // Point at next user buffer address
        Data++;
        //Negate the CS (chip select) to end the SROM command
        outp32(iobase+CSR_9, CSR9_WRITE | SEL_SROM);
        Delay800nSec();
    }
    return 0;
}

