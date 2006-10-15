//$Id$
#include "MonADEC.h"
using namespace MonAPI;
using namespace mones;

MonADEC::~MonADEC()
{
    printf("destructor of monadec\n");
}

MonADEC::MonADEC():rxdsc(NULL),txdsc(NULL),
                   rxindex(0),txindex(0)
{
    memcpy(devname,"dc21140a",9);
}

//Initalization Procedure is described in H.R.M 4.3.4
int MonADEC::init()
{    
  //  disableNetwork();
    AllocateDmaPages( ((1<<LOGRXRINGLEN)+(1<<LOGTXRINGLEN))*(ETHER_MAX_PACKET+sizeof(DESC)) );//pages.
    if( dma_head == NULL ){
        printf("buffer allocation was failed.");
        return -1;
    }
    reset();
    //1. Wait 50 PCI clock cycles.
    sleep(100);    
    //2. Update configuration registers (Section 3.1):
    //   -->already be configured by Dispatch::
    //3. Write CSR0 to set global host bus operating parameters (Section 3.2.2.1).
    outp32(iobase+CSR_0,CSR0_CA32);
    //4. Write CSR7 to mask unnecessary (depending on the particular application) interrupt causes.
    outp32(iobase+CSR_7,CSR7_NI|CSR7_AI|CSR7_ERE|CSR7_FBE|CSR7_GPT|CSR7_ETE|CSR7_RW|
           CSR7_RS|CSR7_RU|CSR7_RI|CSR7_UN|CSR7_TJ|CSR7_TU|CSR7_TS|CSR7_TI);
    //5. The driver must create the transmit and receive descriptor lists. Then, it writes to both CSR3
    //   and CSR4, providing the 21143 with the starting address of each list (Section 3.2.2.7). The
    //   first descriptor on the transmit list may contain a setup frame (Section 4.2.3).
    rxdsc = (DESC*)dma_head;
    rxbuf = dma_head+((1<<LOGRXRINGLEN)*sizeof(DESC));
    for(int i=0;i<(1<<LOGRXRINGLEN);i++){
        (rxdsc+i)->status=RX_OWN;
        (rxdsc+i)->ctlandcnt=RCH|ETHER_MAX_PACKET;
        (rxdsc+i)->bufaddr1=(dword)(rxbuf+i*ETHER_MAX_PACKET);
        (rxdsc+i)->bufaddr2=(dword)(rxdsc+i+1);
    }
    (rxdsc+((1<<LOGRXRINGLEN)-1))->ctlandcnt |= RER;
    (rxdsc+((1<<LOGRXRINGLEN)-1))->bufaddr2=(dword)rxdsc;
    txdsc = (DESC*)(rxbuf+(1<<LOGRXRINGLEN)*ETHER_MAX_PACKET);
    txbuf = (byte*)txdsc+((1<<LOGTXRINGLEN)*sizeof(DESC));
    for(int i=0;i<(1<<LOGTXRINGLEN);i++){
        (txdsc+i)->status=0;
        (txdsc+i)->ctlandcnt=TCH|ETHER_MAX_PACKET;
        (txdsc+i)->bufaddr1=(dword)(txbuf+i*ETHER_MAX_PACKET);
        (txdsc+i)->bufaddr2=(dword)(rxdsc+i+1);
    }
    (txdsc+((1<<LOGTXRINGLEN)-1))->ctlandcnt|=TER; 
    (txdsc+((1<<LOGTXRINGLEN)-1))->bufaddr2=(dword)txdsc; 
    //Get serial rom contents.
    word val[SROM_SIZE];
    if( ReadSROM(SROM_SIZE,val) ){
        return -1;
    }    
    for(int i=0;i<6;i++){
        macaddress[i]=((byte*)(val+MAC_OFFSET))[i]; 
        //mac address of VirtualPC is defined in vmc file.
    }    
    //Create setup frame.
    for(int i=0;i<3;i++){
        *(dword*)(txdsc->bufaddr1+i)=0x0000ffff;
    }    
    for(int i=3;i<SETUPPKTSIZE/4;i++){
        *(dword*)(txdsc->bufaddr1+i)=val[MAC_OFFSET+(i%3)];
    }
    txdsc->status=TX_OWN;
    txdsc->ctlandcnt= SET | SETUPPKTSIZE;
    txindex++;
    outp32(iobase+CSR_3,(dword)rxdsc);
    outp32(iobase+CSR_4,(dword)txdsc);
    //6. Write CSR6 (Section 3.2.2.6) to set global serial parameters and to start both the receive and
    //   transmit processes. The receive and transmit processes enter the running state and attempt to
    //   acquire descriptors from the respective descriptor lists. Then the receive and transmit
    //   processes begin processing incoming and outgoing frames. The receive and transmit.  
    outp32( iobase + CSR_6,CSR6_SR|CSR6_PM|CSR6_FD|CSR6_ST|CSR6_MD1);// ????
    //printf("Hello Virtual PC\n");
    return 0;
}

int MonADEC::interrupt()
{
    //see section 4.3.3
    word ret=0x0000;
    dword val=inp32(iobase+CSR_5);
    if( val & CSR5_TI ){
        txihandler();
        ret |= TX_INT;
    }
    if( val & CSR5_RI ){
        rxihandler();
        ret |= RX_INT;
    }
    outp32(iobase+CSR_5,val);
    enableNetwork();
    return ret;
}


void MonADEC::txihandler()
{ 
    //printf("TX\n");   
    int i=txindex;
    while( ( (txdsc+i)->status & TX_OWN ) == TX_OWN ){
        (txdsc+i)->status=0;
        i--;
        if( i<0 ){i=(( 1<<LOGTXRINGLEN)-1);}
    }
}

void MonADEC::rxihandler()
{
    //printf("RX:%d\n",rxindex);
    word length;
    while( ((rxdsc+rxindex)->status & RX_OWN) == 0 ){
        length=(((rxdsc+rxindex)->ctlandcnt)&0x07FF);
        Ether* frame = new Ether; //deleted by server.
        memcpy(frame,(byte*)((rxdsc+rxindex)->bufaddr1),length);
        //printf("%d\n",length);
        rxFrameList.add(frame);
        (rxdsc+rxindex)->status=RX_OWN;
        (rxdsc+rxindex)->ctlandcnt=RCH|ETHER_MAX_PACKET;
        if( rxindex == ((1<<LOGRXRINGLEN))-1 ){
            (rxdsc+rxindex)->ctlandcnt|=RER;
            rxindex=0;
        }else{
            rxindex++;
        }
    }
}

void MonADEC::SendFrm(Ether* frame)
{
    //printf("send frame\n");  
    //printf("T:%d R:%d\n",txindex,rxindex);
    enableNetwork();
    word len=CalcFrameSize(frame);
    txFrameList.add(frame);
    while( txFrameList.size() != 0 && ( (txdsc+txindex)->status & TX_OWN) != TX_OWN ) {
        Ether* frame = txFrameList.removeAt(0);
        memcpy((void*)((txdsc+txindex)->bufaddr1),frame,len); 
        (txdsc+txindex)->status=TX_OWN;
        (txdsc+txindex)->ctlandcnt=IC|LS|FS|ETHER_MAX_PACKET;//Someting wrong?
        outp32(iobase+CSR_1,0x1);
        if( txindex == ((1<<LOGTXRINGLEN))-1){
            (txdsc+txindex)->ctlandcnt|=TER;
            txindex = 0;
        }else{
            txindex++;
        }
        delete frame;
    }
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

