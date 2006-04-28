#include "MonAMDpcn.h"
using namespace MonAPI;
using namespace mones;

////////
TX::TX() 
{ 
	dsc=NULL;
	buf=monapi_allocate_dma_memory();
	for(int i=1;i<4;i++){
		byte* tmp=monapi_allocate_dma_memory();
		if( tmp != buf+0x1000*i){
			printf("TX:buf is not continuous.%d\n",i);
			buf=NULL;
		}
	}
}

TX::~TX()
{
	for(int i=0;i<4;i++)
		monapi_deallocate_dma_memory(dsc+0x1000*i);
}

int TX::initTX()
{
	if( buf == 0 )
		return -1;
	printf("TX%x\n",buf);
	dsc= (TXDSC*)buf;
	buf += ((1<<LOGRINGLEN)*sizeof(TXDSC));
	for(int i=0;i<(1<<LOGRINGLEN);i++){
		(dsc+i)->status=0;
		(dsc+i)->control=0;
		(dsc+i)->bcnt=(dword)(-PKTSIZE);
		(dsc+i)->rbaddr=(dword)(buf+i*PKTSIZE);
	}
	return 0;
}

void TX::ihandler()
{
	printf("TX-INT\n");
}

////////
RX::RX() 
{  
	dsc=NULL;
	buf=monapi_allocate_dma_memory();
	for( int i=1;i<3;i++){
		byte* tmp=monapi_allocate_dma_memory();
		if( tmp != buf+0x1000*i){
			printf("RX:buf is not continuous.%d\n",i);
			buf=NULL;
		}
	}
}	

RX::~RX()
{
	for(int i=0;i<3;i++)
		monapi_deallocate_dma_memory(dsc+0x1000*i);
	printf("free\n");
}

int RX::initRX() 
{
	if( buf== 0 )
		return -1;
	printf("RX%x %x\n",buf,sizeof(RXDSC));
	dsc = (RXDSC*)buf;
	buf += ((1<<LOGRINGLEN)*sizeof(RXDSC));
	for(int i=0;i<(1<<LOGRINGLEN);i++){
	    (dsc+i)->bcnt=(dword)(-PKTSIZE)|0xF000;
		(dsc+i)->status=RMD1_OWN|RMD1_STP|RMD1_ENP;
		(dsc+i)->rbaddr=(dword)(buf+i*PKTSIZE);
	}
	index=0;
	return 0;
}

void RX::ihandler()
{
	printf("RX%x %x\n",index,(dsc+index)->status);
	for(int i=0;i<(1<<LOGRINGLEN);i++)
	        printf("[%x %x]",(dsc+i)->status,(dsc+i)->mcnt);
	while( ((dsc+index)->status & RMD1_OWN) == 0 ){
		printf("==>%x %x\n",(dsc+index)->rbaddr,dsc);
		(dsc+index)->mcnt=0;
        (dsc+index)->bcnt = (dword)(-PKTSIZE)|0xF000;
		(dsc+index)->status = RMD1_OWN|RMD1_STP|RMD1_ENP;  
		index = (index+1) & ((1<<LOGRINGLEN)-1);
	}

	printf("\n");
}
///////////////////////////////////////////////////////

int MonAMDpcn::init()
{
	stop();
	reset();

	w_bcr(BCR_MISC,BCR_AUTOSEL);	    //SET BCR_EDGETRG for Edge Sense.
	w_bcr(BCR_SSTYLE,BCR_PCI_II|BCR_SSIZE);
	if( RX::initRX() != 0 ){
		printf("RX::SetupFailed.");
		return -1;
	}
	if( TX::initTX() != 0 ){ 
		printf("TX::SetupFailed.");
		return -1;
	}
	//Use initalize block.
	piblock=(IBLK*)monapi_allocate_dma_memory();
	piblock->mode=MODE_DNY_BCST;
	piblock->rxlen=(RX::LOGRINGLEN<<4);  //see page157.
	piblock->txlen=(TX::LOGRINGLEN<<4);
	for(int i=0;i<5;i++){
		piblock->mac_addr[i]=inp8(iobase+i);
		printf("%x:",piblock->mac_addr[i]);
	}
	piblock->mac_addr[5]=inp8(iobase+5);
	printf("%x\n",piblock->mac_addr[5]);
	piblock->filter[0]=0x0;
	piblock->filter[1]=0x0;
	piblock->rx_ring=(dword)RX::dsc;
	piblock->tx_ring=(dword)TX::dsc;
	printf("DMA area was allocated properly.\n");

	w_csr(CSR_IADR0, ((dword)piblock)&0xFFFF);
	w_csr(CSR_IADR1,(((dword)piblock)>>16)&0xFFFF);
	w_csr(CSR_CSR,CSR_INTEN|CSR_INIT);
	sleep(100);
	w_csr(CSR_FEATURE,FEAT_PADTX|FEAT_TXMSK);    //CSR 4
	//printf("chip version=%x\n",(r_csr(88)>>12)|(r_csr(89)<<4));
	w_csr(CSR_CSR,CSR_INTEN|CSR_START);                //CSR 0
	return 0;	
}

void MonAMDpcn::interrupt()
{
	//printf("Interrupted\n");
	word val;
	if( ( val = r_csr(CSR_CSR)) & CSR_INTR ){
		printf("<%x>",val);
		if( val & CSR_RINT ){
			RX::ihandler();
		}else if (val & CSR_TINT){
			TX::ihandler();
		}
		w_csr(CSR_CSR, val & 0xFFF0);
	}
	//Interrupt was masked by OS handler.
	enableNetwork(); //Now be enabled here. 
	//It should be enabled by messaging mechanism. 
}
 
void MonAMDpcn::outputFrame(byte* packet, byte* macAddress, dword size, word protocolId)
{

}

void MonAMDpcn::getFrameBuffer(byte* buffer, dword size)
{

}

/////////////
